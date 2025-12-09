#ifndef CHARGING_STATE_MACHINE_H
#define CHARGING_STATE_MACHINE_H

#include <string>
#include <functional>
#include <memory>
#include <vector>

namespace Wallbox
{

    /**
     * @brief Charging states for the wallbox (State Pattern)
     *
     * Implements the State Pattern to manage charging lifecycle.
     * Each state has specific behaviors and valid transitions.
     */
    enum class ChargingState
    {
        IDLE,      ///< Wallbox ready, no vehicle connected
        PREPARING, ///< Vehicle detected, preparing to charge
        CHARGING,  ///< Active charging in progress
        PAUSED,    ///< Charging paused (user request or power limit)
        FINISHING, ///< Charging complete, finishing session
        ERROR,     ///< Error state, requires intervention
        DISABLED   ///< Wallbox disabled for maintenance
    };

    /**
     * @brief Observer callback for state changes
     * @param oldState Previous charging state
     * @param newState New charging state
     * @param reason Description of why the state changed
     */
    using StateChangeCallback = std::function<void(ChargingState oldState, ChargingState newState, const std::string &reason)>;

    /**
     * @brief State machine for managing charging process
     *
     * Implements State Pattern with Observer Pattern for notifications.
     *
     * Design Patterns:
     * - State Pattern: Encapsulates state-specific behavior
     * - Observer Pattern: Notifies listeners of state changes
     * - Command Pattern: State transitions as commands
     *
     * SOLID Principles:
     * - Single Responsibility: Only manages state transitions
     * - Open/Closed: New states can be added via inheritance
     * - Liskov Substitution: State behaviors are substitutable
     */
    class ChargingStateMachine
    {
    public:
        ChargingStateMachine();
        ~ChargingStateMachine();

        // State queries
        ChargingState getCurrentState() const { return m_currentState; }
        std::string getStateString() const;
        std::string getStateString(ChargingState state) const;

        // State transitions
        bool transitionTo(ChargingState newState, const std::string &reason = "");
        bool canTransitionTo(ChargingState newState) const;

        // Common state changes
        bool startCharging(const std::string &reason = "User requested");
        bool stopCharging(const std::string &reason = "User requested");
        bool pauseCharging(const std::string &reason = "User requested");
        bool resumeCharging(const std::string &reason = "User requested");
        bool enterErrorState(const std::string &reason);
        bool reset();

        // Observer pattern
        void addStateChangeListener(StateChangeCallback callback);
        void clearStateChangeListeners();

        // State predicates
        bool isCharging() const { return m_currentState == ChargingState::CHARGING; }
        bool isIdle() const { return m_currentState == ChargingState::IDLE; }
        bool isError() const { return m_currentState == ChargingState::ERROR; }
        bool isPaused() const { return m_currentState == ChargingState::PAUSED; }
        bool isDisabled() const { return m_currentState == ChargingState::DISABLED; }

    private:
        ChargingState m_currentState;
        std::vector<StateChangeCallback> m_listeners;

        void notifyStateChange(ChargingState oldState, ChargingState newState, const std::string &reason);
        bool isValidTransition(ChargingState from, ChargingState to) const;
    };

} // namespace Wallbox

#endif // CHARGING_STATE_MACHINE_H
