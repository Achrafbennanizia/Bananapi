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
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstdlib>

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
            : m_running(false), m_config(Configuration::getInstance()), m_interactiveMode(false), m_dualMode(false)
        {
            // Open log file
            m_logFile.open("/tmp/wallbox_v3.log", std::ios::out | std::ios::app);
        }

        ~Application()
        {
            if (m_logFile.is_open())
            {
                m_logFile.close();
            }
        }

        /**
         * @brief Initialize the application
         * @param interactiveMode If true, skip HTTP API server initialization (unless dualMode is true)
         * @param dualMode If true, start both API server and interactive terminal
         * @return true if successful
         */
        bool initialize(bool interactiveMode = false, bool dualMode = false)
        {
            m_interactiveMode = interactiveMode;
            m_dualMode = dualMode;

            std::cout << "==================================================" << std::endl;
            std::cout << "  Wallbox Controller v3.0 - With REST API       " << std::endl;
            std::cout << "==================================================" << std::endl;

            // Load configuration from file first, then override with environment
            std::cout << "Loading configuration from config.json..." << std::endl;
            m_config.loadFromFile("config.json");
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

            // Create and start API server if NOT in interactive-only mode (or if in dual mode)
            if (!m_interactiveMode || m_dualMode)
            {
                std::cout << "Starting HTTP API server..." << std::endl;
                // Create and setup API server
                m_apiServer = std::make_unique<HttpApiServer>(m_config.getApiPort());
                m_apiController = std::make_unique<ApiController>(*m_wallboxController);
                m_apiController->setupEndpoints(*m_apiServer);

                if (!m_apiServer->start())
                {
                    std::cerr << "Failed to start HTTP API server" << std::endl;
                    return false;
                }
            }
            else
            {
                std::cout << "Interactive mode enabled - skipping API server" << std::endl;
            }

            displayReadyMessage();
            m_running = true;
            return true;
        }

        /**
         * @brief Run the application main loop (API server mode)
         */
        void run()
        {
            // Just keep running and let HTTP API handle requests
            while (m_running)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        /**
         * @brief Run interactive mode with terminal commands
         */
        void runInteractive()
        {
            logMessage("INFO", "Interactive mode started - commands: enable, disable, start, stop, pause, resume, status, help, quit");

            // Show help at startup
            showHelp();

            std::string input;
            while (m_running)
            {
                std::cout << "> " << std::flush;

                if (!std::getline(std::cin, input))
                {
                    break;
                }

                processCommand(input);
            }
        }

        /**
         * @brief Run dual mode with both HTTP API and interactive terminal
         * The API server runs in a background thread (already started in initialize())
         * while the interactive terminal runs in the main thread
         */
        void runDual()
        {
            logMessage("INFO", "Dual mode started - HTTP API + Interactive terminal");

            // Show help at startup
            showHelp();

            std::string input;
            while (m_running)
            {
                std::cout << "> " << std::flush;

                if (!std::getline(std::cin, input))
                {
                    break;
                }

                processCommand(input);
            }

            // When interactive exits, API server will also stop (m_running = false)
        }

        /**
         * @brief Process interactive command
         */
        void processCommand(const std::string &cmd)
        {
            if (cmd.empty())
                return;

            if (cmd == "enable")
            {
                m_wallboxController->enableWallbox();
                logMessage("CMD", "Wallbox enabled");
                std::cout << "✓ Wallbox enabled" << std::endl;
            }
            else if (cmd == "disable")
            {
                m_wallboxController->disableWallbox();
                logMessage("CMD", "Wallbox disabled");
                std::cout << "✓ Wallbox disabled" << std::endl;
            }
            else if (cmd == "start")
            {
                m_wallboxController->startCharging();
                logMessage("CMD", "Charging started");
                std::cout << "✓ Charging started" << std::endl;
            }
            else if (cmd == "stop")
            {
                m_wallboxController->stopCharging();
                logMessage("CMD", "Charging stopped");
                std::cout << "✓ Charging stopped" << std::endl;
            }
            else if (cmd == "pause")
            {
                m_wallboxController->pauseCharging();
                logMessage("CMD", "Charging paused");
                std::cout << "✓ Charging paused" << std::endl;
            }
            else if (cmd == "resume")
            {
                m_wallboxController->resumeCharging();
                logMessage("CMD", "Charging resumed");
                std::cout << "✓ Charging resumed" << std::endl;
            }
            else if (cmd == "status")
            {
                showStatus();
            }
            else if (cmd.substr(0, 9) == "setrelay ")
            {
                try
                {
                    int pin = std::stoi(cmd.substr(9));
                    if (pin >= 0 && pin <= 27)
                    {
                        setRelayPin(pin);
                        std::cout << "✓ Relay pin changed to: " << pin << std::endl;
                        logMessage("CMD", "Relay pin changed to: " + std::to_string(pin));
                    }
                    else
                    {
                        std::cout << "✗ Invalid pin number. Use 0-27" << std::endl;
                    }
                }
                catch (...)
                {
                    std::cout << "✗ Invalid format. Usage: setrelay <pin_number>" << std::endl;
                }
            }
            else if (cmd == "getpins")
            {
                showPinConfiguration();
            }
            else if (cmd.substr(0, 8) == "setudp ")
            {
                // Format: setudp <address> <listen_port> <send_port>
                std::istringstream iss(cmd.substr(8));
                std::string address;
                int listen_port, send_port;
                if (iss >> address >> listen_port >> send_port)
                {
                    if (listen_port > 0 && listen_port < 65536 && send_port > 0 && send_port < 65536)
                    {
                        std::cout << "✓ UDP config will be: " << address << ":" << listen_port << " -> " << send_port << std::endl;
                        std::cout << "  (Restart required to apply changes)" << std::endl;
                        logMessage("CMD", "UDP config request: " + address + ":" + std::to_string(listen_port) + " -> " + std::to_string(send_port));
                    }
                    else
                    {
                        std::cout << "✗ Invalid port numbers. Use 1-65535" << std::endl;
                    }
                }
                else
                {
                    std::cout << "✗ Invalid format. Usage: setudp <address> <listen_port> <send_port>" << std::endl;
                }
            }
            else if (cmd == "getudp")
            {
                std::cout << "UDP Configuration:" << std::endl;
                std::cout << "  Listen Port: " << m_config.getUdpListenPort() << std::endl;
                std::cout << "  Send Port: " << m_config.getUdpSendPort() << std::endl;
                std::cout << "  Send Address: " << m_config.getUdpSendAddress() << std::endl;
            }
            else if (cmd == "help")
            {
                showHelp();
            }
            else if (cmd == "quit" || cmd == "exit")
            {
                std::cout << "Shutting down..." << std::endl;
                m_running = false;
            }
            else
            {
                logMessage("CMD", "Unknown command: " + cmd);
                std::cout << "Unknown command: " << cmd << " (type 'help' for commands)" << std::endl;
            }
        }

        /**
         * @brief Show current status
         */
        void showStatus()
        {
            std::stringstream ss;
            ss << "State: " << m_wallboxController->getStateString()
               << " | Wallbox: " << (m_wallboxController->isWallboxEnabled() ? "Enabled" : "Disabled")
               << " | Relay: " << (m_wallboxController->isRelayEnabled() ? "ON" : "OFF")
               << " | API: port " << m_config.getApiPort()
               << " | Mode: " << m_config.getModeString();

            logMessage("STATUS", ss.str());

            // Display status in terminal
            std::cout << "\n"
                      << ss.str() << "\n"
                      << std::endl;
        }

        /**
         * @brief Show help message
         */
        void showHelp()
        {
            logMessage("CMD", "Help requested");
            std::cout << "\n=== Commands ===\n";
            std::cout << "  enable          - Enable wallbox\n";
            std::cout << "  disable         - Disable wallbox\n";
            std::cout << "  start           - Start charging\n";
            std::cout << "  stop            - Stop charging\n";
            std::cout << "  pause           - Pause charging\n";
            std::cout << "  resume          - Resume charging\n";
            std::cout << "  status          - Show status\n";
            std::cout << "  setrelay <pin>  - Change relay pin number (0-27)\n";
            std::cout << "  getpins         - Show current pin configuration\n";
            std::cout << "  getudp          - Show UDP configuration\n";
            std::cout << "  setudp <addr> <listen> <send> - Set UDP config (needs restart)\n";
            std::cout << "  help            - Show this help\n";
            std::cout << "  quit            - Exit\n";
            std::cout << "================\n"
                      << std::endl;
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

        /**
         * @brief Change relay pin at runtime
         */
        void setRelayPin(int pin)
        {
            m_config.setRelayPin(pin);
            // Note: Changing pin requires reinitializing GPIO, which would need controller restart
            // For now, we just update the config. Full implementation would need GPIO reinitialization.
        }

        /**
         * @brief Show current pin configuration
         */
        void showPinConfiguration()
        {
            std::cout << "\n=== GPIO Pin Configuration ===\n";
            std::cout << "  Relay Enable: " << m_config.getRelayPin() << "\n";
            std::cout << "  LED Green:    " << m_config.getLedGreenPin() << "\n";
            std::cout << "  LED Yellow:   " << m_config.getLedYellowPin() << "\n";
            std::cout << "  LED Red:      " << m_config.getLedRedPin() << "\n";
            std::cout << "  Button:       " << m_config.getButtonPin() << "\n";
            std::cout << "==============================\n"
                      << std::endl;

            logMessage("INFO", "Pin configuration displayed");
        }

    private:
        std::atomic<bool> m_running;
        bool m_interactiveMode;
        bool m_dualMode;
        Configuration &m_config;
        std::unique_ptr<WallboxController> m_wallboxController;
        std::unique_ptr<HttpApiServer> m_apiServer;
        std::unique_ptr<ApiController> m_apiController;
        std::ofstream m_logFile;

        /**
         * @brief Get timestamp string
         */
        std::string getTimestamp()
        {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                          now.time_since_epoch()) %
                      1000;

            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
            ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
            return ss.str();
        }

        /**
         * @brief Log message to file
         */
        void logMessage(const std::string &level, const std::string &message)
        {
            if (m_logFile.is_open())
            {
                m_logFile << "[" << getTimestamp() << "] [" << level << "] " << message << std::endl;
                m_logFile.flush();
            }
        }

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
