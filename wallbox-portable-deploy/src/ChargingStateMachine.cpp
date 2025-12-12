#include "ChargingStateMachine.h"
#include <iostream>
#include <stdexcept>

namespace Wallbox
{

    ChargingStateMachine::ChargingStateMachine()
        : m_currentState(ChargingState::IDLE)
    {
    }

    ChargingStateMachine::~ChargingStateMachine()
    {
        m_listeners.clear();
    }

    std::string ChargingStateMachine::getStateString() const
    {
        return getStateString(m_currentState);
    }

    std::string ChargingStateMachine::getStateString(ChargingState state) const
    {
        switch (state)
        {
        case ChargingState::IDLE:
            return "IDLE";
        case ChargingState::PREPARING:
            return "PREPARING";
        case ChargingState::CHARGING:
            return "CHARGING";
        case ChargingState::PAUSED:
            return "PAUSED";
        case ChargingState::FINISHING:
            return "FINISHING";
        case ChargingState::ERROR:
            return "ERROR";
        case ChargingState::DISABLED:
            return "DISABLED";
        default:
            return "UNKNOWN";
        }
    }

    bool ChargingStateMachine::transitionTo(ChargingState newState, const std::string &reason)
    {
        if (m_currentState == newState)
        {
            // Already in target state
            return true;
        }

        if (!isValidTransition(m_currentState, newState))
        {
            std::cerr << "Invalid state transition: " << getStateString(m_currentState)
                      << " -> " << getStateString(newState) << std::endl;
            return false;
        }

        ChargingState oldState = m_currentState;
        m_currentState = newState;

        std::cout << "State transition: " << getStateString(oldState)
                  << " -> " << getStateString(newState);
        if (!reason.empty())
        {
            std::cout << " (" << reason << ")";
        }
        std::cout << std::endl;

        notifyStateChange(oldState, newState, reason);
        return true;
    }

    bool ChargingStateMachine::canTransitionTo(ChargingState newState) const
    {
        return isValidTransition(m_currentState, newState);
    }

    bool ChargingStateMachine::startCharging(const std::string &reason)
    {
        if (m_currentState == ChargingState::IDLE)
        {
            return transitionTo(ChargingState::PREPARING, reason) &&
                   transitionTo(ChargingState::CHARGING, reason);
        }
        else if (m_currentState == ChargingState::PREPARING)
        {
            return transitionTo(ChargingState::CHARGING, reason);
        }
        return false;
    }

    bool ChargingStateMachine::stopCharging(const std::string &reason)
    {
        if (m_currentState == ChargingState::CHARGING ||
            m_currentState == ChargingState::PAUSED)
        {
            return transitionTo(ChargingState::FINISHING, reason) &&
                   transitionTo(ChargingState::IDLE, reason);
        }
        return false;
    }

    bool ChargingStateMachine::pauseCharging(const std::string &reason)
    {
        if (m_currentState == ChargingState::CHARGING)
        {
            return transitionTo(ChargingState::PAUSED, reason);
        }
        return false;
    }

    bool ChargingStateMachine::resumeCharging(const std::string &reason)
    {
        if (m_currentState == ChargingState::PAUSED)
        {
            return transitionTo(ChargingState::CHARGING, reason);
        }
        return false;
    }

    bool ChargingStateMachine::enterErrorState(const std::string &reason)
    {
        return transitionTo(ChargingState::ERROR, reason);
    }

    bool ChargingStateMachine::reset()
    {
        if (m_currentState == ChargingState::ERROR)
        {
            return transitionTo(ChargingState::IDLE, "Reset from error");
        }
        return false;
    }

    void ChargingStateMachine::addStateChangeListener(StateChangeCallback callback)
    {
        m_listeners.push_back(callback);
    }

    void ChargingStateMachine::clearStateChangeListeners()
    {
        m_listeners.clear();
    }

    void ChargingStateMachine::notifyStateChange(ChargingState oldState, ChargingState newState, const std::string &reason)
    {
        for (const auto &listener : m_listeners)
        {
            listener(oldState, newState, reason);
        }
    }

    bool ChargingStateMachine::isValidTransition(ChargingState from, ChargingState to) const
    {
        // State transition matrix
        switch (from)
        {
        case ChargingState::IDLE:
            return to == ChargingState::PREPARING ||
                   to == ChargingState::DISABLED ||
                   to == ChargingState::ERROR;

        case ChargingState::PREPARING:
            return to == ChargingState::CHARGING ||
                   to == ChargingState::IDLE ||
                   to == ChargingState::ERROR;

        case ChargingState::CHARGING:
            return to == ChargingState::PAUSED ||
                   to == ChargingState::FINISHING ||
                   to == ChargingState::ERROR;

        case ChargingState::PAUSED:
            return to == ChargingState::CHARGING ||
                   to == ChargingState::FINISHING ||
                   to == ChargingState::ERROR;

        case ChargingState::FINISHING:
            return to == ChargingState::IDLE ||
                   to == ChargingState::ERROR;

        case ChargingState::ERROR:
            return to == ChargingState::IDLE ||
                   to == ChargingState::DISABLED;

        case ChargingState::DISABLED:
            return to == ChargingState::IDLE;

        default:
            return false;
        }
    }

} // namespace Wallbox