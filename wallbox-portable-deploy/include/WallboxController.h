#ifndef WALLBOX_CONTROLLER_H
#define WALLBOX_CONTROLLER_H

#include "IGpioController.h"
#include "INetworkCommunicator.h"
#include "ChargingStateMachine.h"
#include "../../LibPubWallbox/IsoStackCtrlProtocol.h"
#include <memory>
#include <string>
#include <atomic>

namespace Wallbox
{

    /**
     * @brief Main controller for the wallbox system
     *
     * Coordinates all subsystems using dependency injection.
     *
     * Design Patterns:
     * - Facade Pattern: Simplifies complex subsystem interactions
     * - Dependency Injection: Constructor injection for loose coupling
     * - Observer Pattern: Responds to state changes
     *
     * SOLID Principles:
     * - Single Responsibility: Coordinates subsystems only
     * - Dependency Inversion: Depends on abstractions (interfaces)
     * - Open/Closed: Extensible through interface implementations
     *
     * Note: Pin definitions moved to Configuration class for centralization
     */
    class WallboxController
    {
    public:
        /**
         * @brief Construct wallbox controller with dependencies
         * @param gpio GPIO controller (Strategy Pattern)
         * @param network Network communicator (Strategy Pattern)
         */
        WallboxController(std::unique_ptr<IGpioController> gpio,
                          std::unique_ptr<INetworkCommunicator> network);

        ~WallboxController();

        // Lifecycle
        bool initialize();
        void shutdown();
        bool isRunning() const { return m_running; }

        // Main control loop
        void run();
        void stop();

        // Charging control
        bool startCharging();
        bool stopCharging();
        bool pauseCharging();
        bool resumeCharging();

        // State queries
        ChargingState getCurrentState() const;
        std::string getStateString() const;
        bool isRelayEnabled() const { return m_relayEnabled; }
        bool isWallboxEnabled() const { return m_wallboxEnabled; }

        // System control
        bool enableWallbox();
        bool disableWallbox();
        bool setRelayState(bool enabled);

        // API for external control (React app, etc.)
        std::string getStatusJson() const;

    private:
        // Dependencies (Dependency Injection)
        std::unique_ptr<IGpioController> m_gpio;
        std::unique_ptr<INetworkCommunicator> m_network;
        std::unique_ptr<ChargingStateMachine> m_stateMachine;

        // State
        std::atomic<bool> m_running;
        bool m_relayEnabled;
        bool m_wallboxEnabled;

        // Private methods
        void setupGpio();
        void updateLeds();
        void sendStatusToSimulator();
        void processNetworkMessage(const std::vector<uint8_t> &message);
        void onStateChange(ChargingState oldState, ChargingState newState, const std::string &reason);

        // LED control
        void setLedState(int pin, bool on);
        void showIdleLeds();
        void showChargingLeds();
        void showErrorLeds();
        void showPausedLeds();
    };

} // namespace Wallbox

#endif // WALLBOX_CONTROLLER_H
