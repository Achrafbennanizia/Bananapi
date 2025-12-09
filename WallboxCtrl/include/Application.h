#ifndef APPLICATION_H
#define APPLICATION_H

#include "Configuration.h"
#include "WallboxController.h"
#include "HttpApiServer.h"
#include "ApiController.h"
#include "GpioFactory.h"
#include "UdpCommunicator.h"
#include <memory>
#include <atomic>
#include <csignal>
#include <iostream>

namespace Wallbox
{

    /**
     * @brief Main application class
     *
     * Orchestrates the entire application lifecycle using the
     * Application Controller pattern. Separates application
     * initialization and lifecycle management from business logic.
     *
     * Design Patterns:
     * - Application Controller: Manages app lifecycle
     * - Facade: Simplifies startup/shutdown
     * - Dependency Injection: Injects all dependencies
     *
     * SOLID:
     * - Single Responsibility: Application lifecycle only
     * - Dependency Inversion: Uses interfaces
     */
    class Application
    {
    public:
        Application()
            : m_running(false), m_config(Configuration::getInstance())
        {
        }

        /**
         * @brief Initialize the application
         * @return true if successful
         */
        bool initialize()
        {
            std::cout << "==================================================" << std::endl;
            std::cout << "  Wallbox Controller v2.0 - With REST API       " << std::endl;
            std::cout << "==================================================" << std::endl;

            // Load configuration
            m_config.loadFromEnvironment();

            displayConfiguration();

            // Create dependencies using factories
            auto gpio = GpioFactory::create(m_config.getGpioType());
            auto network = std::make_unique<UdpCommunicator>(
                m_config.getUdpListenPort(),
                m_config.getUdpSendPort(),
                m_config.getUdpSendAddress());

            // Create wallbox controller
            m_wallboxController = std::make_unique<WallboxController>(
                std::move(gpio),
                std::move(network));

            // Initialize controller
            if (!m_wallboxController->initialize())
            {
                std::cerr << "Failed to initialize wallbox controller" << std::endl;
                return false;
            }

            // Create and setup API server
            m_apiServer = std::make_unique<HttpApiServer>(m_config.getApiPort());
            m_apiController = std::make_unique<ApiController>(*m_wallboxController);
            m_apiController->setupEndpoints(*m_apiServer);

            if (!m_apiServer->start())
            {
                std::cerr << "Failed to start HTTP API server" << std::endl;
                return false;
            }

            displayReadyMessage();
            m_running = true;
            return true;
        }

        /**
         * @brief Run the application main loop
         */
        void run()
        {
            while (m_running)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        /**
         * @brief Shutdown the application gracefully
         */
        void shutdown()
        {
            if (!m_running)
                return;

            std::cout << "\nInitiating shutdown sequence..." << std::endl;

            if (m_apiServer)
            {
                m_apiServer->stop();
            }

            if (m_wallboxController)
            {
                m_wallboxController->shutdown();
            }

            m_running = false;
            std::cout << "Wallbox controller stopped cleanly." << std::endl;
        }

        /**
         * @brief Request application shutdown (thread-safe)
         */
        void requestShutdown()
        {
            m_running = false;
        }

        /**
         * @brief Check if application is running
         */
        bool isRunning() const
        {
            return m_running;
        }

    private:
        std::atomic<bool> m_running;
        Configuration &m_config;
        std::unique_ptr<WallboxController> m_wallboxController;
        std::unique_ptr<HttpApiServer> m_apiServer;
        std::unique_ptr<ApiController> m_apiController;

        void displayConfiguration()
        {
            std::cout << "\n╔════════════════════════════════════════════════╗" << std::endl;
            std::cout << "║  MODE: " << (m_config.isDevelopmentMode() ? "DEVELOPMENT 🔧" : "PRODUCTION ⚡ ") << "                        ║" << std::endl;
            std::cout << "╚════════════════════════════════════════════════╝" << std::endl;

            std::cout << "\nConfiguration:" << std::endl;
            std::cout << "  Mode: " << m_config.getModeString() << std::endl;
            std::cout << "  GPIO Type: " << m_config.getGpioType()
                      << (m_config.isDevelopmentMode() ? " (simulator)" : " (real pins)") << std::endl;
            std::cout << "  UDP Listen Port: " << m_config.getUdpListenPort() << std::endl;
            std::cout << "  UDP Send Port: " << m_config.getUdpSendPort() << std::endl;
            std::cout << "  UDP Send Address: " << m_config.getUdpSendAddress() << std::endl;
            std::cout << "  REST API Port: " << m_config.getApiPort() << std::endl;

            if (m_config.isDevelopmentMode())
            {
                std::cout << "\n💡 Development Mode Active:" << std::endl;
                std::cout << "  ✓ Using GPIO simulator (no real hardware)" << std::endl;
                std::cout << "  ✓ React app: http://localhost:3000" << std::endl;
                std::cout << "  ✓ API server: http://localhost:" << m_config.getApiPort() << std::endl;
                std::cout << "  ✓ Simulator: UDP on ports " << m_config.getUdpListenPort()
                          << "/" << m_config.getUdpSendPort() << std::endl;
            }
            else
            {
                std::cout << "\n⚡ Production Mode Active:" << std::endl;
                std::cout << "  ✓ Using real Banana Pi GPIO pins" << std::endl;
                std::cout << "  ⚠️  WARNING: This will control real hardware!" << std::endl;
            }
            std::cout << std::endl;
        }

        void displayReadyMessage()
        {
            std::cout << "\n╔════════════════════════════════════════════════╗" << std::endl;
            std::cout << "║  Wallbox Controller Ready                      ║" << std::endl;
            std::cout << "╠════════════════════════════════════════════════╣" << std::endl;
            std::cout << "║  Current state: " << m_wallboxController->getStateString();
            for (size_t i = m_wallboxController->getStateString().length(); i < 30; i++)
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
            std::cout << "║  React App URL: http://localhost:" << m_config.getApiPort() << "         ║" << std::endl;
            std::cout << "║                                                ║" << std::endl;
            std::cout << "║  Press Ctrl+C to exit                          ║" << std::endl;
            std::cout << "╚════════════════════════════════════════════════╝" << std::endl;
            std::cout << std::endl;
        }
    };

} // namespace Wallbox

#endif // APPLICATION_H
