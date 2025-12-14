#include "HardwareCpSignalReader.h"
#include <iostream>
#include <chrono>
#include <thread>

namespace Wallbox
{

    HardwareCpSignalReader::HardwareCpSignalReader(std::shared_ptr<IGpioController> gpio, int cpPin)
        : m_gpio(gpio), m_cpPin(cpPin), m_initialized(false), m_monitoring(false), m_currentState(CpState::UNKNOWN)
    {
    }

    HardwareCpSignalReader::~HardwareCpSignalReader()
    {
        shutdown();
    }

    bool HardwareCpSignalReader::initialize()
    {
        if (m_initialized)
        {
            return true;
        }

        if (!m_gpio || !m_gpio->isInitialized())
        {
            std::cerr << "[HardwareCpSignalReader] GPIO controller not initialized" << std::endl;
            return false;
        }

        // Configure CP pin as input
        if (!m_gpio->setPinMode(m_cpPin, PinMode::INPUT))
        {
            std::cerr << "[HardwareCpSignalReader] Failed to configure CP pin " << m_cpPin << " as input" << std::endl;
            return false;
        }

        m_initialized = true;
        m_currentState = readCpState();
        std::cout << "[HardwareCpSignalReader] Initialized on pin " << m_cpPin
                  << ", initial state: " << getCpStateString(m_currentState) << std::endl;
        return true;
    }

    void HardwareCpSignalReader::shutdown()
    {
        stopMonitoring();
        m_initialized = false;
    }

    CpState HardwareCpSignalReader::readCpState()
    {
        if (!m_initialized)
        {
            return CpState::UNKNOWN;
        }

        // Read voltage from CP pin
        int voltage = readVoltage();
        return voltageToState(voltage);
    }

    int HardwareCpSignalReader::readVoltage()
    {
        // In a real implementation, this would read from an ADC
        // For now, we simulate by reading digital pin state and mapping
        PinValue value = m_gpio->digitalRead(m_cpPin);

        // Simulate voltage based on digital reading
        // In real hardware, use ADC to read actual voltage
        if (value == PinValue::HIGH)
        {
            return 12000; // 12V - STATE_A (no vehicle)
        }
        else
        {
            return 6000; // 6V - STATE_C (ready to charge)
        }
    }

    CpState HardwareCpSignalReader::voltageToState(int voltage)
    {
        // IEC 61851-1 voltage thresholds (in mV)
        if (voltage > 11000)
        {
            return CpState::STATE_A; // 12V - No vehicle
        }
        else if (voltage > 8000)
        {
            return CpState::STATE_B; // 9V - Vehicle connected, not ready
        }
        else if (voltage > 5000)
        {
            return CpState::STATE_C; // 6V - Vehicle ready to charge
        }
        else if (voltage > 2000)
        {
            return CpState::STATE_D; // 3V - Ventilation required
        }
        else if (voltage > 500)
        {
            return CpState::STATE_E; // 0V - No power
        }
        else if (voltage < -10000)
        {
            return CpState::STATE_F; // -12V - Error
        }
        else
        {
            return CpState::UNKNOWN;
        }
    }

    std::string HardwareCpSignalReader::getCpStateString(CpState state) const
    {
        switch (state)
        {
        case CpState::STATE_A:
            return "STATE_A (No vehicle, 12V)";
        case CpState::STATE_B:
            return "STATE_B (Vehicle connected, 9V)";
        case CpState::STATE_C:
            return "STATE_C (Ready to charge, 6V)";
        case CpState::STATE_D:
            return "STATE_D (Ventilation required, 3V)";
        case CpState::STATE_E:
            return "STATE_E (No power, 0V)";
        case CpState::STATE_F:
            return "STATE_F (Error, -12V)";
        case CpState::UNKNOWN:
        default:
            return "UNKNOWN";
        }
    }

    void HardwareCpSignalReader::onStateChange(CpStateChangeCallback callback)
    {
        m_callbacks.push_back(callback);
    }

    void HardwareCpSignalReader::startMonitoring()
    {
        if (m_monitoring.load())
        {
            return;
        }

        if (!m_initialized)
        {
            std::cerr << "[HardwareCpSignalReader] Cannot start monitoring, not initialized" << std::endl;
            return;
        }

        m_monitoring.store(true);
        m_monitorThread = std::thread(&HardwareCpSignalReader::monitorLoop, this);
        std::cout << "[HardwareCpSignalReader] Started monitoring CP signal" << std::endl;
    }

    void HardwareCpSignalReader::stopMonitoring()
    {
        if (!m_monitoring.load())
        {
            return;
        }

        m_monitoring.store(false);
        if (m_monitorThread.joinable())
        {
            m_monitorThread.join();
        }
        std::cout << "[HardwareCpSignalReader] Stopped monitoring CP signal" << std::endl;
    }

    void HardwareCpSignalReader::monitorLoop()
    {
        while (m_monitoring.load())
        {
            CpState newState = readCpState();
            if (newState != m_currentState && newState != CpState::UNKNOWN)
            {
                CpState oldState = m_currentState;
                m_currentState = newState;
                std::cout << "[HardwareCpSignalReader] CP state changed: "
                          << getCpStateString(oldState) << " -> "
                          << getCpStateString(newState) << std::endl;
                notifyStateChange(oldState, newState);
            }

            // Poll every 100ms
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void HardwareCpSignalReader::notifyStateChange(CpState oldState, CpState newState)
    {
        for (const auto &callback : m_callbacks)
        {
            callback(oldState, newState);
        }
    }

} // namespace Wallbox
