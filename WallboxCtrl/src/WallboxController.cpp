#include "WallboxController.h"
#include "Configuration.h"
#include "../../LibPubWallbox/IsoStackCtrlProtocol.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>
#include <ctime>
#include <cstring>

using namespace Iso15118;

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

        auto lastStatusSend = std::chrono::steady_clock::now();
        const auto statusInterval = std::chrono::milliseconds(100); // Send status every 100ms to match simulator

        while (m_running)
        {
            // Update LEDs based on current state
            updateLeds();

            // Send status to simulator periodically
            auto now = std::chrono::steady_clock::now();
            if (now - lastStatusSend >= statusInterval)
            {
                sendStatusToSimulator();
                lastStatusSend = now;
            }

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
            std::cerr << "\n⚠️  Cannot start charging: wallbox is disabled" << std::endl;
            std::cout << "\n[WALLBOX] ❌ Command rejected - wallbox disabled" << std::endl;
            std::cout << "> " << std::flush;
            return false;
        }

        if (!m_stateMachine->startCharging("User requested"))
        {
            return false;
        }

        std::cout << "\n[WALLBOX → SIMULATOR] ✓ Starting charging sequence" << std::endl;
        std::cout << "> " << std::flush;

        // Enable relay for charging
        return setRelayState(true);
    }

    bool WallboxController::stopCharging()
    {
        if (!m_wallboxEnabled)
        {
            std::cerr << "\n⚠️  Cannot stop charging: wallbox is disabled" << std::endl;
            std::cout << "> " << std::flush;
            return false;
        }

        if (!m_stateMachine->stopCharging("User requested"))
        {
            return false;
        }

        std::cout << "\n[WALLBOX → SIMULATOR] Stopping charging" << std::endl;
        std::cout << "> " << std::flush;

        // Disable relay
        return setRelayState(false);
    }

    bool WallboxController::pauseCharging()
    {
        if (!m_wallboxEnabled)
        {
            std::cerr << "\n⚠️  Cannot pause charging: wallbox is disabled" << std::endl;
            std::cout << "> " << std::flush;
            return false;
        }

        std::cout << "\n[WALLBOX → SIMULATOR] Pausing charging" << std::endl;
        std::cout << "> " << std::flush;
        return m_stateMachine->pauseCharging("User requested");
    }

    bool WallboxController::resumeCharging()
    {
        if (!m_wallboxEnabled)
        {
            std::cerr << "\n⚠️  Cannot resume charging: wallbox is disabled" << std::endl;
            std::cout << "> " << std::flush;
            return false;
        }

        std::cout << "\n[WALLBOX → SIMULATOR] Resuming charging" << std::endl;
        std::cout << "> " << std::flush;
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
        std::cout << "\n[WALLBOX] 🟢 Wallbox ENABLED - Ready for charging" << std::endl;
        std::cout << "> " << std::flush;
        updateLeds();
        return true;
    }

    bool WallboxController::disableWallbox()
    {
        // Stop charging if active
        if (m_stateMachine->isCharging())
        {
            std::cout << "\n[WALLBOX] Stopping active charging before disable..." << std::endl;
            stopCharging();
        }

        m_wallboxEnabled = false;
        std::cout << "\n[WALLBOX] 🔴 Wallbox DISABLED - All charging commands blocked" << std::endl;
        std::cout << "> " << std::flush;
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
        std::cout << "\n[WALLBOX → SIMULATOR] Relay state: "
                  << (enabled ? "ON" : "OFF") << std::endl;
        std::cout << "> " << std::flush;
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
        case ChargingState::OFF:
            showErrorLeds(); // OFF state shows as error (no power)
            break;
        case ChargingState::IDLE:
            showIdleLeds();
            break;
        case ChargingState::CONNECTED:
        case ChargingState::IDENTIFICATION:
        case ChargingState::READY:
        case ChargingState::CHARGING:
            showChargingLeds();
            break;
        case ChargingState::STOP:
        case ChargingState::FINISHED:
            showIdleLeds(); // Back to idle state
            break;
        case ChargingState::ERROR:
            showErrorLeds();
            break;
        default:
            break;
        }
    }

    void WallboxController::sendStatusToSimulator()
    {
        using namespace Iso15118;

        stSeIsoStackCmd cmd;
        cmd.isoStackCmd.msgVersion = 0;
        cmd.isoStackCmd.msgType = enIsoStackMsgType::SeCtrlCmd;
        cmd.isoStackCmd.enable = m_wallboxEnabled ? uint8_t(1) : uint8_t(0);

        // Map wallbox charging state to current demand (use as state indicator)
        // We'll use currentDemand field to communicate the desired state
        ChargingState currentState = m_stateMachine->getCurrentState();
        switch (currentState)
        {
        case ChargingState::OFF:
            cmd.isoStackCmd.currentDemand = 0; // 0 = off
            break;
        case ChargingState::IDLE:
            cmd.isoStackCmd.currentDemand = 10; // 10 = idle
            break;
        case ChargingState::CONNECTED:
            cmd.isoStackCmd.currentDemand = 20; // 20 = connected
            break;
        case ChargingState::IDENTIFICATION:
            cmd.isoStackCmd.currentDemand = 30; // 30 = identification
            break;
        case ChargingState::READY:
            cmd.isoStackCmd.currentDemand = 100; // 100 = ready
            break;
        case ChargingState::CHARGING:
            cmd.isoStackCmd.currentDemand = 160; // 160 = charging (16.0A)
            break;
        case ChargingState::STOP:
            cmd.isoStackCmd.currentDemand = 5; // 5 = stop
            break;
        case ChargingState::FINISHED:
            cmd.isoStackCmd.currentDemand = 1; // 1 = finished
            break;
        default:
            cmd.isoStackCmd.currentDemand = 0;
            break;
        }

        cmd.seHardwareState.mainContactor = m_relayEnabled ? uint8_t(1) : uint8_t(0);

        // Debug output
        static bool firstSend = true;
        static bool lastSentEnable = true;
        static bool lastSentRelay = false;
        static ChargingState lastSentState = ChargingState::IDLE;
        static int sendCount = 0;

        if (firstSend)
        {
            std::cout << "\n[WALLBOX] ✓ Starting to send status to simulator" << std::endl;
            std::cout << "  Initial state: enable=" << (m_wallboxEnabled ? "true" : "false")
                      << " relay=" << (m_relayEnabled ? "ON" : "OFF")
                      << " state=" << getStateString() << std::endl;
            std::cout << "> " << std::flush;
            firstSend = false;
        }

        if (m_wallboxEnabled != lastSentEnable)
        {
            std::cout << "\n[WALLBOX → SIMULATOR] Sending enable status: "
                      << (m_wallboxEnabled ? "ENABLED" : "DISABLED") << std::endl;
            std::cout << "> " << std::flush;
            lastSentEnable = m_wallboxEnabled;
        }

        if (m_relayEnabled != lastSentRelay)
        {
            std::cout << "\n[WALLBOX → SIMULATOR] Sending relay status: "
                      << (m_relayEnabled ? "ON" : "OFF") << std::endl;
            std::cout << "> " << std::flush;
            lastSentRelay = m_relayEnabled;
        }

        if (currentState != lastSentState)
        {
            std::cout << "\n[WALLBOX → SIMULATOR] Sending state change: "
                      << m_stateMachine->getStateString(lastSentState) << " → "
                      << getStateString() << std::endl;
            std::cout << "> " << std::flush;
            lastSentState = currentState;
        }

        // Log every 50th send to confirm communication is active
        sendCount++;
        if (sendCount % 50 == 0)
        {
            // Silent log to file only - no console spam
        }

        // Send via network
        std::vector<uint8_t> message(sizeof(cmd));
        std::memcpy(message.data(), &cmd, sizeof(cmd));
        m_network->send(message);
    }

    void WallboxController::processNetworkMessage(const std::vector<uint8_t> &message)
    {
        // Parse and handle network messages from simulator
        if (message.size() >= sizeof(stSeIsoStackState))
        {
            stSeIsoStackState state;
            std::memcpy(&state, message.data(), sizeof(state));

            // Show feedback when receiving simulator state
            static enIsoChargingState lastState = enIsoChargingState::idle;
            static bool lastContactor = false;
            static bool lastEnableCmd = true;

            bool contactorCmd = (state.seHardwareCmd.mainContactor != 0);
            bool enableCmd = (state.seHardwareCmd.sourceEnable != 0);

            if (state.isoStackState.state != lastState || contactorCmd != lastContactor || enableCmd != lastEnableCmd)
            {
                std::cout << "\n[SIMULATOR → WALLBOX] ";

                if (enableCmd != lastEnableCmd)
                {
                    std::cout << "Enable: " << (lastEnableCmd ? "true" : "false")
                              << " → " << (enableCmd ? "true" : "false") << "  ";

                    if (enableCmd && !m_wallboxEnabled)
                    {
                        std::cout << "\n[WALLBOX] 🟢 Enable requested by simulator";
                        enableWallbox();
                    }
                    else if (!enableCmd && m_wallboxEnabled)
                    {
                        std::cout << "\n[WALLBOX] 🔴 Disable requested by simulator";
                        disableWallbox();
                    }
                }

                if (state.isoStackState.state != lastState)
                {
                    std::cout << "State: " << enIsoChargingState_toString(lastState)
                              << " → " << enIsoChargingState_toString(state.isoStackState.state) << "  ";

                    // Enforce state transition order: idle → ready → charging
                    // stop can be called from any state
                    ChargingState currentWallboxState = m_stateMachine->getCurrentState();

                    switch (state.isoStackState.state)
                    {
                    case enIsoChargingState::idle:
                        // idle can transition from any state (always allowed)
                        if (currentWallboxState != ChargingState::IDLE)
                        {
                            std::cout << "\n[WALLBOX] 🔄 Transitioning to IDLE";
                            m_stateMachine->stopCharging("Simulator state: idle");
                        }
                        break;

                    case enIsoChargingState::ready:
                        // ready can only be reached from idle AND relay must be ON
                        if (!m_relayEnabled)
                        {
                            std::cout << "\n[WALLBOX] ❌ Cannot go to READY: Relay must be ON first";
                        }
                        else if (currentWallboxState == ChargingState::IDLE)
                        {
                            std::cout << "\n[WALLBOX] ✓ Vehicle ready - prepared for charging";
                            // No state machine transition needed, just acknowledgment
                        }
                        else
                        {
                            std::cout << "\n[WALLBOX] ❌ Cannot go to READY: Must be in IDLE state first";
                        }
                        break;

                    case enIsoChargingState::charging:
                        // charging can only be reached from ready (via idle) AND relay must be ON
                        if (!m_relayEnabled)
                        {
                            std::cout << "\n[WALLBOX] ❌ Cannot start charging: Relay must be ON";
                        }
                        else if (currentWallboxState == ChargingState::IDLE && lastState == enIsoChargingState::ready)
                        {
                            if (m_wallboxEnabled)
                            {
                                std::cout << "\n[WALLBOX] 🔄 Starting charging (idle → ready → charging)";
                                m_stateMachine->startCharging("Simulator state: charging");
                            }
                            else
                            {
                                std::cout << "\n[WALLBOX] ❌ Cannot start charging: Wallbox disabled";
                            }
                        }
                        else if (currentWallboxState == ChargingState::CHARGING)
                        {
                            // Already charging, OK
                        }
                        else
                        {
                            std::cout << "\n[WALLBOX] ❌ Cannot start charging: Must go idle → ready → charge";
                        }
                        break;

                    case enIsoChargingState::stop:
                        // stop can be called from any state (always allowed)
                        if (m_stateMachine->isCharging())
                        {
                            std::cout << "\n[WALLBOX] 🔄 Stopping charging (stop command)";
                            m_stateMachine->stopCharging("Simulator state: stop");
                        }
                        break;

                    default:
                        break;
                    }
                }

                if (contactorCmd != lastContactor)
                {
                    std::cout << "Contactor: " << (lastContactor ? "ON" : "OFF")
                              << " → " << (contactorCmd ? "ON" : "OFF");

                    // Apply contactor command only if wallbox is enabled
                    if (!m_wallboxEnabled && contactorCmd)
                    {
                        std::cout << " ❌ REJECTED (wallbox disabled)";
                    }
                    else
                    {
                        // Apply the contactor command
                        if (contactorCmd && !m_relayEnabled)
                        {
                            std::cout << "\n[WALLBOX] ⚡ Activating contactor";
                            setRelayState(true);
                        }
                        else if (!contactorCmd && m_relayEnabled)
                        {
                            std::cout << "\n[WALLBOX] 🔌 Deactivating contactor";
                            setRelayState(false);
                        }
                    }
                }

                std::cout << std::endl
                          << "> " << std::flush;

                lastState = state.isoStackState.state;
                lastContactor = contactorCmd;
                lastEnableCmd = enableCmd;

                // Send immediate status update when state changes
                sendStatusToSimulator();
            }
        }
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
