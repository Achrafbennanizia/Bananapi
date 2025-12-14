#include "WallboxController.h"
#include "StubGpioController.h"
#include "BananaPiGpioController.h"
#include "UdpCommunicator.h"
#include "HttpApiServer.h"
#include <iostream>
#include <memory>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <cstdlib>

using namespace Wallbox;

// Global flag for signal handling
static std::atomic<bool> g_shutdownRequested(false);

/**
 * @brief Signal handler for graceful shutdown
 */
void signalHandler(int signal)
{
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
    g_shutdownRequested = true;
}

/**
 * @brief Factory function for creating GPIO controller
 * @param type Type of GPIO controller ("stub" for development, "bananapi" for production)
 */
std::unique_ptr<IGpioController> createGpioController(const std::string &type)
{
    if (type == "stub")
    {
        std::cout << "[GPIO Factory] Creating stub GPIO controller (development mode)" << std::endl;
        return std::make_unique<StubGpioController>();
    }
    else if (type == "bananapi" || type == "real")
    {
        std::cout << "[GPIO Factory] Creating Banana Pi GPIO controller (production mode)" << std::endl;
        return std::make_unique<BananaPiGpioController>();
    }

    std::cerr << "[GPIO Factory] Unknown GPIO type: " << type << ", using stub" << std::endl;
    return std::make_unique<StubGpioController>();
}

/**
 * @brief Setup REST API endpoints for React app control
 */
void setupApiEndpoints(HttpApiServer &api, WallboxController &controller)
{
    // GET /api/status - Get current wallbox status
    api.GET("/api/status", [&controller](const HttpRequest &, HttpResponse &res)
            { res.setJson(controller.getStatusJson()); });

    // POST /api/charging/start - Start charging
    api.POST("/api/charging/start", [&controller](const HttpRequest &, HttpResponse &res)
             {
        if (controller.startCharging()) {
            JsonBuilder json;
            json.add("success", true)
                .add("message", "Charging started")
                .add("state", controller.getStateString());
            res.setJson(json.build());
        } else {
            res.setError(400, "Failed to start charging");
        } });

    // POST /api/charging/stop - Stop charging
    api.POST("/api/charging/stop", [&controller](const HttpRequest &, HttpResponse &res)
             {
        if (controller.stopCharging()) {
            JsonBuilder json;
            json.add("success", true)
                .add("message", "Charging stopped")
                .add("state", controller.getStateString());
            res.setJson(json.build());
        } else {
            res.setError(400, "Failed to stop charging");
        } });

    // POST /api/charging/pause - Pause charging
    api.POST("/api/charging/pause", [&controller](const HttpRequest &, HttpResponse &res)
             {
        if (controller.pauseCharging()) {
            JsonBuilder json;
            json.add("success", true)
                .add("message", "Charging paused")
                .add("state", controller.getStateString());
            res.setJson(json.build());
        } else {
            res.setError(400, "Failed to pause charging");
        } });

    // POST /api/charging/resume - Resume charging
    api.POST("/api/charging/resume", [&controller](const HttpRequest &, HttpResponse &res)
             {
        if (controller.resumeCharging()) {
            JsonBuilder json;
            json.add("success", true)
                .add("message", "Charging resumed")
                .add("state", controller.getStateString());
            res.setJson(json.build());
        } else {
            res.setError(400, "Failed to resume charging");
        } });

    // POST /api/wallbox/enable - Enable wallbox
    api.POST("/api/wallbox/enable", [&controller](const HttpRequest &, HttpResponse &res)
             {
        if (controller.enableWallbox()) {
            JsonBuilder json;
            json.add("success", true)
                .add("message", "Wallbox enabled")
                .add("enabled", true);
            res.setJson(json.build());
        } else {
            res.setError(400, "Failed to enable wallbox");
        } });

    // POST /api/wallbox/disable - Disable wallbox
    api.POST("/api/wallbox/disable", [&controller](const HttpRequest &, HttpResponse &res)
             {
        if (controller.disableWallbox()) {
            JsonBuilder json;
            json.add("success", true)
                .add("message", "Wallbox disabled")
                .add("enabled", false);
            res.setJson(json.build());
        } else {
            res.setError(400, "Failed to disable wallbox");
        } });

    // GET /api/relay - Get relay status
    api.GET("/api/relay", [&controller](const HttpRequest &, HttpResponse &res)
            {
        JsonBuilder json;
        json.add("relayEnabled", controller.isRelayEnabled())
            .add("state", controller.getStateString());
        res.setJson(json.build()); });

    // GET /health - Health check for React app
    api.GET("/health", [](const HttpRequest &, HttpResponse &res)
            {
        JsonBuilder json;
        json.add("status", "healthy")
            .add("service", "Wallbox Controller API")
            .add("version", "2.0.0");
        res.setJson(json.build()); });
}

/**
 * @brief Main entry point with REST API for React app integration
 */
int main(int argc, char *argv[])
{
    (void)argc; // Suppress unused warning
    (void)argv;

    std::cout << "==================================================" << std::endl;
    std::cout << "  Wallbox Controller v2.0 - With REST API       " << std::endl;
    std::cout << "==================================================" << std::endl;

    // Register signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    try
    {
        // Configuration - Check for development mode
        // Set WALLBOX_MODE=dev for development (simulator + React app)
        // Set WALLBOX_MODE=prod for production (real pins)
        const char *modeEnv = std::getenv("WALLBOX_MODE");
        std::string mode = modeEnv ? std::string(modeEnv) : "dev";

        bool isDevelopment = (mode == "dev" || mode == "development");
        std::string gpioType = isDevelopment ? "stub" : "bananapi";

        const int UDP_LISTEN_PORT = 50010;
        const int UDP_SEND_PORT = 50011;
        const std::string UDP_SEND_ADDRESS = "127.0.0.1";
        const int API_PORT = 8080;

        std::cout << "\n╔════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║  MODE: " << (isDevelopment ? "DEVELOPMENT 🔧" : "PRODUCTION ⚡ ") << "                        ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════╝" << std::endl;

        std::cout << "\nConfiguration:" << std::endl;
        std::cout << "  Mode: " << mode << std::endl;
        std::cout << "  GPIO Type: " << gpioType << (isDevelopment ? " (simulator)" : " (real pins)") << std::endl;
        std::cout << "  UDP Listen Port: " << UDP_LISTEN_PORT << std::endl;
        std::cout << "  UDP Send Port: " << UDP_SEND_PORT << std::endl;
        std::cout << "  UDP Send Address: " << UDP_SEND_ADDRESS << std::endl;
        std::cout << "  REST API Port: " << API_PORT << std::endl;

        if (isDevelopment)
        {
            std::cout << "\n💡 Development Mode Active:" << std::endl;
            std::cout << "  ✓ Using GPIO simulator (no real hardware)" << std::endl;
            std::cout << "  ✓ React app: http://localhost:3000" << std::endl;
            std::cout << "  ✓ API server: http://localhost:" << API_PORT << std::endl;
            std::cout << "  ✓ Simulator: UDP on ports " << UDP_LISTEN_PORT << "/" << UDP_SEND_PORT << std::endl;
        }
        else
        {
            std::cout << "\n⚡ Production Mode Active:" << std::endl;
            std::cout << "  ✓ Using real Banana Pi GPIO pins" << std::endl;
            std::cout << "  ⚠️  WARNING: This will control real hardware!" << std::endl;
        }
        std::cout << std::endl;

        // Create dependencies
        auto gpio = createGpioController(gpioType);
        auto network = std::make_unique<UdpCommunicator>(UDP_LISTEN_PORT, UDP_SEND_PORT, UDP_SEND_ADDRESS);

        // Create controller
        WallboxController controller(std::move(gpio), std::move(network));

        // Initialize controller
        if (!controller.initialize())
        {
            std::cerr << "Failed to initialize wallbox controller" << std::endl;
            return 1;
        }

        // Create and start HTTP API server
        HttpApiServer apiServer(API_PORT);
        setupApiEndpoints(apiServer, controller);

        if (!apiServer.start())
        {
            std::cerr << "Failed to start HTTP API server" << std::endl;
            return 1;
        }

        std::cout << "\n╔════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║  Wallbox Controller Ready                      ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║  Current state: " << controller.getStateString();
        for (size_t i = controller.getStateString().length(); i < 30; i++)
            std::cout << " ";
        std::cout << "║" << std::endl;
        std::cout << "║                                                ║" << std::endl;
        std::cout << "║  REST API Endpoints:                           ║" << std::endl;
        std::cout << "║  • GET  /api/status                            ║" << std::endl;
        std::cout << "║  • GET  /api/relay                             ║" << std::endl;
        std::cout << "║  • POST /api/charging/start                    ║" << std::endl;
        std::cout << "║  • POST /api/charging/stop                     ║" << std::endl;
        std::cout << "║  • POST /api/charging/pause                    ║" << std::endl;
        std::cout << "║  • POST /api/charging/resume                   ║" << std::endl;
        std::cout << "║  • POST /api/wallbox/enable                    ║" << std::endl;
        std::cout << "║  • POST /api/wallbox/disable                   ║" << std::endl;
        std::cout << "║  • GET  /health                                ║" << std::endl;
        std::cout << "║                                                ║" << std::endl;
        std::cout << "║  React App URL: http://localhost:" << API_PORT << "         ║" << std::endl;
        std::cout << "║                                                ║" << std::endl;
        std::cout << "║  Press Ctrl+C to exit                          ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;

        // Main loop
        while (!g_shutdownRequested)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // Graceful shutdown
        std::cout << "\nInitiating shutdown sequence..." << std::endl;
        apiServer.stop();
        controller.shutdown();

        std::cout << "Wallbox controller stopped cleanly." << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 1;
    }
}
