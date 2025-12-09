#include "WallboxController.h"
#include "Configuration.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>
#include <ctime>

namespace Wallbox
{

    WallboxController::WallboxController(std::unique_ptr<IGpioController> gpio,
                                         std::unique_ptr<INetworkCommunicator> network)
        : m_gpio(std::move(gpio)), m_network(std::move(network)), m_stateMachine(std::make_unique<ChargingStateMachine>()), m_running(false), m_relayEnabled(false), m_wallboxEnabled(true)
    {
        // Register for state change notifications (Observer Pattern)
        m_stateMachine->addStateChangeListener(
            [this](ChargingState oldState, ChargingState newState, const std::string &reason)
            {
                onStateChange(oldState, newState, reason);
            });
    }

    WallboxController::~WallboxController()
    {
        shutdown();
    }

    bool WallboxController::initialize()
    {
        std::cout << "Initializing Wallbox Controller..." << std::endl;

        // Initialize GPIO
        if (!m_gpio->initialize())
        {
            std::cerr << "Failed to initialize GPIO" << std::endl;
            return false;
        }

        setupGpio();

        // Initialize network
        if (!m_network->connect())
        {
            std::cerr << "Failed to initialize network" << std::endl;
            return false;
        }

        // Start receiving network messages
        m_network->startReceiving([this](const std::vector<uint8_t> &message)
                                  { processNetworkMessage(message); });

        // Initial LED state
        updateLeds();

        std::cout << "Wallbox Controller initialized successfully" << std::endl;
        return true;
    }

    void WallboxController::shutdown()
    {
        std::cout << "Shutting down Wallbox Controller..." << std::endl;

        m_running = false;

        // Stop charging if active
        if (m_stateMachine->isCharging())
        {
            stopCharging();
        }

        // Disable relay
        setRelayState(false);

        // Shutdown network
        if (m_network)
        {
            m_network->stopReceiving();
            m_network->disconnect();
        }

        // Shutdown GPIO
        if (m_gpio)
        {
            m_gpio->shutdown();
        }

        std::cout << "Wallbox Controller shutdown complete" << std::endl;
    }

    void WallboxController::run()
    {
        m_running = true;

        std::cout << "Wallbox Controller running..." << std::endl;

        while (m_running)
        {
            // Update LEDs based on current state
            updateLeds();

            // Check for button press (if implemented)
            // PinValue buttonState = m_gpio->digitalRead(Configuration::Pins::BUTTON);

            // Small delay to prevent CPU spinning
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void WallboxController::stop()
    {
        m_running = false;
    }

    bool WallboxController::startCharging()
    {
        if (!m_wallboxEnabled)
        {
            std::cerr << "Cannot start charging: wallbox is disabled" << std::endl;
            return false;
        }

        if (!m_stateMachine->startCharging("User requested"))
        {
            return false;
        }

        // Enable relay for charging
        return setRelayState(true);
    }

    bool WallboxController::stopCharging()
    {
        if (!m_stateMachine->stopCharging("User requested"))
        {
            return false;
        }

        // Disable relay
        return setRelayState(false);
    }

    bool WallboxController::pauseCharging()
    {
        return m_stateMachine->pauseCharging("User requested");
    }

    bool WallboxController::resumeCharging()
    {
        if (!m_wallboxEnabled)
        {
            std::cerr << "Cannot resume charging: wallbox is disabled" << std::endl;
            return false;
        }

        return m_stateMachine->resumeCharging("User requested");
    }

    ChargingState WallboxController::getCurrentState() const
    {
        return m_stateMachine->getCurrentState();
    }

    std::string WallboxController::getStateString() const
    {
        return m_stateMachine->getStateString();
    }

    bool WallboxController::enableWallbox()
    {
        m_wallboxEnabled = true;
        std::cout << "Wallbox enabled" << std::endl;
        updateLeds();
        return true;
    }

    bool WallboxController::disableWallbox()
    {
        // Stop charging if active
        if (m_stateMachine->isCharging())
        {
            stopCharging();
        }

        m_wallboxEnabled = false;
        std::cout << "Wallbox disabled" << std::endl;
        updateLeds();
        return true;
    }

    bool WallboxController::setRelayState(bool enabled)
    {
        PinValue value = enabled ? PinValue::HIGH : PinValue::LOW;

        if (!m_gpio->digitalWrite(Configuration::Pins::RELAY_ENABLE, value))
        {
            std::cerr << "Failed to set relay state" << std::endl;
            return false;
        }

        m_relayEnabled = enabled;
        std::cout << "Relay " << (enabled ? "enabled" : "disabled") << std::endl;
        return true;
    }

    std::string WallboxController::getStatusJson() const
    {
        std::ostringstream json;
        json << "{"
             << "\"state\":\"" << getStateString() << "\","
             << "\"wallboxEnabled\":" << (m_wallboxEnabled ? "true" : "false") << ","
             << "\"relayEnabled\":" << (m_relayEnabled ? "true" : "false") << ","
             << "\"charging\":" << (m_stateMachine->isCharging() ? "true" : "false") << ","
             << "\"timestamp\":" << std::time(nullptr)
             << "}";
        return json.str();
    }

    void WallboxController::setupGpio()
    {
        // Configure output pins
        m_gpio->setPinMode(Configuration::Pins::RELAY_ENABLE, PinMode::OUTPUT);
        m_gpio->setPinMode(Configuration::Pins::LED_GREEN, PinMode::OUTPUT);
        m_gpio->setPinMode(Configuration::Pins::LED_YELLOW, PinMode::OUTPUT);
        m_gpio->setPinMode(Configuration::Pins::LED_RED, PinMode::OUTPUT);

        // Configure input pins
        m_gpio->setPinMode(Configuration::Pins::BUTTON, PinMode::INPUT);

        // Initialize outputs to OFF
        m_gpio->digitalWrite(Configuration::Pins::RELAY_ENABLE, PinValue::LOW);
        m_gpio->digitalWrite(Configuration::Pins::LED_GREEN, PinValue::LOW);
        m_gpio->digitalWrite(Configuration::Pins::LED_YELLOW, PinValue::LOW);
        m_gpio->digitalWrite(Configuration::Pins::LED_RED, PinValue::LOW);
    }

    void WallboxController::updateLeds()
    {
        if (!m_wallboxEnabled)
        {
            // All LEDs off when disabled
            setLedState(Configuration::Pins::LED_GREEN, false);
            setLedState(Configuration::Pins::LED_YELLOW, false);
            setLedState(Configuration::Pins::LED_RED, false);
            return;
        }

        switch (m_stateMachine->getCurrentState())
        {
        case ChargingState::IDLE:
            showIdleLeds();
            break;
        case ChargingState::PREPARING:
        case ChargingState::CHARGING:
            showChargingLeds();
            break;
        case ChargingState::PAUSED:
            showPausedLeds();
            break;
        case ChargingState::ERROR:
        case ChargingState::DISABLED:
            showErrorLeds();
            break;
        default:
            break;
        }
    }

    void WallboxController::processNetworkMessage(const std::vector<uint8_t> &message)
    {
        // Parse and handle network messages
        // This would implement the protocol from IsoStackCtrlProtocol
        std::cout << "Received network message (" << message.size() << " bytes)" << std::endl;

        // Example: Simple command processing
        // In a real implementation, this would parse the protocol messages
    }

    void WallboxController::onStateChange(ChargingState oldState, ChargingState newState, const std::string &reason)
    {
        std::cout << "Controller responding to state change: "
                  << m_stateMachine->getStateString(oldState) << " -> "
                  << m_stateMachine->getStateString(newState) << std::endl;

        // Update LEDs when state changes
        updateLeds();

        // Send state change notification over network
        // (Implementation would depend on protocol)
    }

    void WallboxController::setLedState(int pin, bool on)
    {
        m_gpio->digitalWrite(pin, on ? PinValue::HIGH : PinValue::LOW);
    }

    void WallboxController::showIdleLeds()
    {
        setLedState(Configuration::Pins::LED_GREEN, true);   // Green ON
        setLedState(Configuration::Pins::LED_YELLOW, false); // Yellow OFF
        setLedState(Configuration::Pins::LED_RED, false);    // Red OFF
    }

    void WallboxController::showChargingLeds()
    {
        setLedState(Configuration::Pins::LED_GREEN, false); // Green OFF
        setLedState(Configuration::Pins::LED_YELLOW, true); // Yellow ON
        setLedState(Configuration::Pins::LED_RED, false);   // Red OFF
    }

    void WallboxController::showErrorLeds()
    {
        setLedState(Configuration::Pins::LED_GREEN, false);  // Green OFF
        setLedState(Configuration::Pins::LED_YELLOW, false); // Yellow OFF
        setLedState(Configuration::Pins::LED_RED, true);     // Red ON
    }

    void WallboxController::showPausedLeds()
    {
        setLedState(Configuration::Pins::LED_GREEN, false); // Green OFF
        setLedState(Configuration::Pins::LED_YELLOW, true); // Yellow ON (blinking could be implemented)
        setLedState(Configuration::Pins::LED_RED, true);    // Red ON
    }

} // namespace Wallbox
