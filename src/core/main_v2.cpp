#include "WallboxController.h"
#include "StubGpioController.h"
#include "UdpCommunicator.h"
#include <iostream>
#include <memory>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>

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
 *
 * Implements Factory Pattern for platform-specific GPIO creation.
 *
 * @param type Type of GPIO controller ("stub", "wiringpi", "sysfs")
 * @return Unique pointer to GPIO controller
 */
std::unique_ptr<IGpioController> createGpioController(const std::string &type)
{
    if (type == "stub")
    {
        return std::make_unique<StubGpioController>();
    }
    // Add other implementations here:
    // else if (type == "wiringpi") {
    //     return std::make_unique<WiringPiGpioController>();
    // }

    std::cerr << "Unknown GPIO type: " << type << ", using stub" << std::endl;
    return std::make_unique<StubGpioController>();
}

/**
 * @brief Main entry point for wallbox controller
 *
 * Demonstrates Dependency Injection and Factory patterns.
 *
 * Architecture:
 * 1. Create dependencies (GPIO, Network) using factories
 * 2. Inject dependencies into WallboxController
 * 3. Run controller main loop
 * 4. Graceful shutdown on signal
 */
int main(int argc, char *argv[])
{
    std::cout << "==================================================" << std::endl;
    std::cout << "  Wallbox Controller v2.0 - SOLID Architecture   " << std::endl;
    std::cout << "==================================================" << std::endl;

    // Register signal handlers for graceful shutdown
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    try
    {
        // Configuration (could be loaded from file in production)
        const int LISTEN_PORT = 50010;
        const int SEND_PORT = 50011;
        const std::string SEND_ADDRESS = "127.0.0.1";
        const std::string GPIO_TYPE = "stub"; // Change to "wiringpi" on Banana Pi

        std::cout << "\nConfiguration:" << std::endl;
        std::cout << "  Listen Port: " << LISTEN_PORT << std::endl;
        std::cout << "  Send Port: " << SEND_PORT << std::endl;
        std::cout << "  Send Address: " << SEND_ADDRESS << std::endl;
        std::cout << "  GPIO Type: " << GPIO_TYPE << std::endl;
        std::cout << std::endl;

        // Create dependencies using factories (Factory Pattern + Dependency Injection)
        auto gpio = createGpioController(GPIO_TYPE);
        auto network = std::make_unique<UdpCommunicator>(LISTEN_PORT, SEND_PORT, SEND_ADDRESS);

        // Inject dependencies into controller
        WallboxController controller(std::move(gpio), std::move(network));

        // Initialize system
        if (!controller.initialize())
        {
            std::cerr << "Failed to initialize wallbox controller" << std::endl;
            return 1;
        }

        std::cout << "\nWallbox controller running. Press Ctrl+C to exit." << std::endl;
        std::cout << "Current state: " << controller.getStateString() << std::endl;
        std::cout << std::endl;

        // Main loop (runs until shutdown signal)
        while (!g_shutdownRequested && controller.isRunning())
        {
            // Controller handles its own event loop
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // Graceful shutdown
        std::cout << "\nInitiating shutdown sequence..." << std::endl;
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
