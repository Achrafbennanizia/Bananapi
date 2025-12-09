#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <cstdlib>

namespace Wallbox
{

    /**
     * @brief Configuration management using Singleton pattern
     *
     * Centralizes all application configuration, making it easy to manage
     * settings from environment variables, config files, or command-line args.
     *
     * Design Pattern: Singleton
     * SOLID: Single Responsibility (only manages configuration)
     */
    class Configuration
    {
    public:
        /**
         * @brief Operating mode enumeration
         */
        enum class Mode
        {
            DEVELOPMENT,
            PRODUCTION
        };

        /**
         * @brief Get singleton instance
         */
        static Configuration &getInstance()
        {
            static Configuration instance;
            return instance;
        }

        // Delete copy and move constructors
        Configuration(const Configuration &) = delete;
        Configuration &operator=(const Configuration &) = delete;
        Configuration(Configuration &&) = delete;
        Configuration &operator=(Configuration &&) = delete;

        // Mode
        Mode getMode() const { return m_mode; }
        bool isDevelopmentMode() const { return m_mode == Mode::DEVELOPMENT; }
        bool isProductionMode() const { return m_mode == Mode::PRODUCTION; }
        std::string getModeString() const
        {
            return m_mode == Mode::DEVELOPMENT ? "development" : "production";
        }

        // GPIO
        std::string getGpioType() const
        {
            return m_mode == Mode::DEVELOPMENT ? "stub" : "bananapi";
        }

        // Network
        int getUdpListenPort() const { return m_udpListenPort; }
        int getUdpSendPort() const { return m_udpSendPort; }
        std::string getUdpSendAddress() const { return m_udpSendAddress; }

        // API
        int getApiPort() const { return m_apiPort; }

        // GPIO Pins
        struct Pins
        {
            static constexpr int RELAY_ENABLE = 4;
            static constexpr int LED_GREEN = 17;
            static constexpr int LED_YELLOW = 27;
            static constexpr int LED_RED = 22;
            static constexpr int BUTTON = 23;
        };

        // Load configuration from environment or defaults
        void loadFromEnvironment()
        {
            // Mode
            const char *modeEnv = std::getenv("WALLBOX_MODE");
            std::string modeStr = modeEnv ? std::string(modeEnv) : "dev";
            m_mode = (modeStr == "prod" || modeStr == "production")
                         ? Mode::PRODUCTION
                         : Mode::DEVELOPMENT;

            // Ports (can be overridden by env vars)
            const char *apiPortEnv = std::getenv("WALLBOX_API_PORT");
            if (apiPortEnv)
            {
                m_apiPort = std::stoi(apiPortEnv);
            }

            const char *udpListenEnv = std::getenv("WALLBOX_UDP_LISTEN_PORT");
            if (udpListenEnv)
            {
                m_udpListenPort = std::stoi(udpListenEnv);
            }
        }

    private:
        Configuration()
            : m_mode(Mode::DEVELOPMENT),
              m_udpListenPort(50010),
              m_udpSendPort(50011),
              m_udpSendAddress("127.0.0.1"),
              m_apiPort(8080)
        {
        }

        Mode m_mode;
        int m_udpListenPort;
        int m_udpSendPort;
        std::string m_udpSendAddress;
        int m_apiPort;
    };

} // namespace Wallbox

#endif // CONFIGURATION_H
