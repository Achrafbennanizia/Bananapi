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
        case ChargingState::OFF:
            return "OFF";
        case ChargingState::IDLE:
            return "IDLE";
        case ChargingState::CONNECTED:
            return "CONNECTED";
        case ChargingState::IDENTIFICATION:
            return "IDENTIFICATION";
        case ChargingState::READY:
            return "READY";
        case ChargingState::CHARGING:
            return "CHARGING";
        case ChargingState::STOP:
            return "STOP";
        case ChargingState::FINISHED:
            return "FINISHED";
        case ChargingState::ERROR:
            return "ERROR";
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
            return transitionTo(ChargingState::CONNECTED, reason) &&
                   transitionTo(ChargingState::IDENTIFICATION, reason) &&
                   transitionTo(ChargingState::READY, reason) &&
                   transitionTo(ChargingState::CHARGING, reason);
        }
        else if (m_currentState == ChargingState::CONNECTED)
        {
            return transitionTo(ChargingState::IDENTIFICATION, reason) &&
                   transitionTo(ChargingState::READY, reason) &&
                   transitionTo(ChargingState::CHARGING, reason);
        }
        else if (m_currentState == ChargingState::IDENTIFICATION)
        {
            return transitionTo(ChargingState::READY, reason) &&
                   transitionTo(ChargingState::CHARGING, reason);
        }
        else if (m_currentState == ChargingState::READY)
        {
            return transitionTo(ChargingState::CHARGING, reason);
        }
        return false;
    }

    bool ChargingStateMachine::stopCharging(const std::string &reason)
    {
        if (m_currentState == ChargingState::CHARGING ||
            m_currentState == ChargingState::READY)
        {
            return transitionTo(ChargingState::STOP, reason) &&
                   transitionTo(ChargingState::FINISHED, reason) &&
                   transitionTo(ChargingState::IDLE, reason);
        }
        return false;
    }

    bool ChargingStateMachine::pauseCharging(const std::string &reason)
    {
        if (m_currentState == ChargingState::CHARGING)
        {
            return transitionTo(ChargingState::READY, reason);
        }
        return false;
    }

    bool ChargingStateMachine::resumeCharging(const std::string &reason)
    {
        if (m_currentState == ChargingState::READY)
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
        // State transition matrix following ISO 15118 standard
        switch (from)
        {
        case ChargingState::OFF:
            return to == ChargingState::IDLE ||
                   to == ChargingState::ERROR;

        case ChargingState::IDLE:
            return to == ChargingState::CONNECTED ||
                   to == ChargingState::OFF ||
                   to == ChargingState::ERROR;

        case ChargingState::CONNECTED:
            return to == ChargingState::IDENTIFICATION ||
                   to == ChargingState::IDLE ||
                   to == ChargingState::ERROR;

        case ChargingState::IDENTIFICATION:
            return to == ChargingState::READY ||
                   to == ChargingState::IDLE ||
                   to == ChargingState::ERROR;

        case ChargingState::READY:
            return to == ChargingState::CHARGING ||
                   to == ChargingState::STOP ||
                   to == ChargingState::IDLE ||
                   to == ChargingState::ERROR;

        case ChargingState::CHARGING:
            return to == ChargingState::READY || // Pause
                   to == ChargingState::STOP ||
                   to == ChargingState::ERROR;

        case ChargingState::STOP:
            return to == ChargingState::FINISHED ||
                   to == ChargingState::ERROR;

        case ChargingState::FINISHED:
            return to == ChargingState::IDLE ||
                   to == ChargingState::ERROR;

        case ChargingState::ERROR:
            return to == ChargingState::IDLE ||
                   to == ChargingState::OFF;

        default:
            return false;
        }
    }

} // namespace Wallbox