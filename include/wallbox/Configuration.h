#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>

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

        // GPIO Pins - now configurable
        int getRelayPin() const { return m_relayPin; }
        int getLedGreenPin() const { return m_ledGreenPin; }
        int getLedYellowPin() const { return m_ledYellowPin; }
        int getLedRedPin() const { return m_ledRedPin; }
        int getButtonPin() const { return m_buttonPin; }
        int getCpPin() const { return m_cpPin; }

        // Setters for runtime configuration
        void setRelayPin(int pin) { m_relayPin = pin; }
        void setLedGreenPin(int pin) { m_ledGreenPin = pin; }
        void setLedYellowPin(int pin) { m_ledYellowPin = pin; }
        void setLedRedPin(int pin) { m_ledRedPin = pin; }
        void setButtonPin(int pin) { m_buttonPin = pin; }
        void setCpPin(int pin) { m_cpPin = pin; }

        // Charging parameters
        int getMaxCurrentAmps() const { return m_maxCurrentAmps; }
        int getVoltage() const { return m_voltage; }
        int getTimeoutSeconds() const { return m_timeoutSeconds; }

        // Logging
        std::string getLogFile() const { return m_logFile; }
        std::string getLogLevel() const { return m_logLevel; }

        // Legacy GPIO Pins struct for backward compatibility
        // Updated for BananaPi M5 sysfs GPIO numbers
        struct Pins
        {
            static constexpr int RELAY_ENABLE = 586; // Physical Pin 21
            static constexpr int LED_GREEN = 587;    // Physical Pin 24
            static constexpr int LED_YELLOW = 590;   // Physical Pin 10
            static constexpr int LED_RED = 579;      // Physical Pin 22
            static constexpr int BUTTON = 588;       // Physical Pin 23
        };

        // CP (Control Pilot) Pin - for IEC 61851-1 signal reading
        static constexpr int CP_PIN = 585; // GPIO pin for CP signal (Physical Pin 19)

        // Load configuration from environment or defaults
        // Note: Environment variables OVERRIDE config file settings
        void loadFromEnvironment()
        {
            // Mode - only override if WALLBOX_MODE is explicitly set
            const char *modeEnv = std::getenv("WALLBOX_MODE");
            if (modeEnv)
            {
                std::string modeStr(modeEnv);
                m_mode = (modeStr == "prod" || modeStr == "production")
                             ? Mode::PRODUCTION
                             : Mode::DEVELOPMENT;
            }

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

        // Load configuration from JSON file
        bool loadFromFile(const std::string &filepath)
        {
            std::ifstream file(filepath);
            if (!file.is_open())
            {
                std::cerr << "Warning: Could not open config file: " << filepath << std::endl;
                std::cerr << "Using default configuration." << std::endl;
                return false;
            }

            std::string content((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
            file.close();

            // Simple JSON parser (basic implementation)
            parseJson(content);
            return true;
        }

    private:
        Configuration()
            : m_mode(Mode::DEVELOPMENT),
              m_udpListenPort(50010),
              m_udpSendPort(50011),
              m_udpSendAddress("127.0.0.1"),
              m_apiPort(8080),
              m_relayPin(21), // v4.0 default: GPIO 21
              m_ledGreenPin(17),
              m_ledYellowPin(27),
              m_ledRedPin(22),
              m_buttonPin(23),
              m_cpPin(7), // CP signal pin (ADC capable)
              m_maxCurrentAmps(16),
              m_voltage(230),
              m_timeoutSeconds(300),
              m_logFile("/tmp/wallbox_v4.log"),
              m_logLevel("info")
        {
        }

        // Simple JSON parser
        void parseJson(const std::string &content)
        {
            // Parse mode
            std::string modeValue = extractJsonValue(content, "mode");
            if (!modeValue.empty())
            {
                m_mode = (modeValue == "production") ? Mode::PRODUCTION : Mode::DEVELOPMENT;
            }

            // Parse network settings
            m_udpListenPort = extractJsonInt(content, "udp_listen_port", m_udpListenPort);
            m_udpSendPort = extractJsonInt(content, "udp_send_port", m_udpSendPort);
            m_apiPort = extractJsonInt(content, "api_port", m_apiPort);
            std::string addr = extractJsonValue(content, "udp_send_address");
            if (!addr.empty())
                m_udpSendAddress = addr;

            // Parse GPIO pins
            m_relayPin = extractJsonInt(content, "relay_enable", m_relayPin);
            m_ledGreenPin = extractJsonInt(content, "led_green", m_ledGreenPin);
            m_ledYellowPin = extractJsonInt(content, "led_yellow", m_ledYellowPin);
            m_ledRedPin = extractJsonInt(content, "led_red", m_ledRedPin);
            m_buttonPin = extractJsonInt(content, "button", m_buttonPin);
            m_cpPin = extractJsonInt(content, "cp_pin", m_cpPin);

            // Parse charging parameters
            m_maxCurrentAmps = extractJsonInt(content, "max_current_amps", m_maxCurrentAmps);
            m_voltage = extractJsonInt(content, "voltage", m_voltage);
            m_timeoutSeconds = extractJsonInt(content, "timeout_seconds", m_timeoutSeconds);

            // Parse logging
            std::string logFile = extractJsonValue(content, "\"file\"");
            if (!logFile.empty())
                m_logFile = logFile;
            std::string logLevel = extractJsonValue(content, "level");
            if (!logLevel.empty())
                m_logLevel = logLevel;
        }

        std::string extractJsonValue(const std::string &json, const std::string &key)
        {
            std::string searchKey = "\"" + key + "\"";
            size_t pos = json.find(searchKey);
            if (pos == std::string::npos)
                return "";

            pos = json.find(":", pos);
            if (pos == std::string::npos)
                return "";

            pos = json.find("\"", pos);
            if (pos == std::string::npos)
                return "";

            size_t end = json.find("\"", pos + 1);
            if (end == std::string::npos)
                return "";

            return json.substr(pos + 1, end - pos - 1);
        }

        int extractJsonInt(const std::string &json, const std::string &key, int defaultValue)
        {
            std::string searchKey = "\"" + key + "\"";
            size_t pos = json.find(searchKey);
            if (pos == std::string::npos)
                return defaultValue;

            pos = json.find(":", pos);
            if (pos == std::string::npos)
                return defaultValue;

            // Skip whitespace
            pos++;
            while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n'))
                pos++;

            // Extract number
            size_t end = pos;
            while (end < json.length() && (isdigit(json[end]) || json[end] == '-'))
                end++;

            if (end > pos)
            {
                try
                {
                    return std::stoi(json.substr(pos, end - pos));
                }
                catch (...)
                {
                    return defaultValue;
                }
            }

            return defaultValue;
        }

        Mode m_mode;
        int m_udpListenPort;
        int m_udpSendPort;
        std::string m_udpSendAddress;
        int m_apiPort;

        // GPIO Pins
        int m_relayPin;
        int m_ledGreenPin;
        int m_ledYellowPin;
        int m_ledRedPin;
        int m_buttonPin;
        int m_cpPin;

        // Charging parameters
        int m_maxCurrentAmps;
        int m_voltage;
        int m_timeoutSeconds;

        // Logging
        std::string m_logFile;
        std::string m_logLevel;
    };

} // namespace Wallbox

#endif // CONFIGURATION_H
