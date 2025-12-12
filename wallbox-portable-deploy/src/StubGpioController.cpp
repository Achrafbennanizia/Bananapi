#include "StubGpioController.h"
#include <iostream>

namespace Wallbox
{

    StubGpioController::StubGpioController()
    {
        std::cout << "StubGpioController: Initialized (no hardware access)" << std::endl;
    }

    StubGpioController::~StubGpioController()
    {
        std::cout << "StubGpioController: Shutdown" << std::endl;
    }

    bool StubGpioController::initialize()
    {
        std::cout << "StubGpioController: GPIO initialization (simulated)" << std::endl;
        return true;
    }

    void StubGpioController::shutdown()
    {
        m_pinStates.clear();
        std::cout << "StubGpioController: GPIO shutdown (simulated)" << std::endl;
    }

    bool StubGpioController::setPinMode(int pin, PinMode mode)
    {
        std::string modeStr = (mode == PinMode::INPUT) ? "INPUT" : "OUTPUT";
        std::cout << "StubGpioController: Set pin " << pin << " to " << modeStr << std::endl;
        return true;
    }

    bool StubGpioController::digitalWrite(int pin, PinValue value)
    {
        std::string valueStr = (value == PinValue::HIGH) ? "HIGH" : "LOW";
        std::cout << "StubGpioController: Write pin " << pin << " = " << valueStr << std::endl;

        // Store state for reading back
        m_pinStates[pin] = value;
        return true;
    }

    PinValue StubGpioController::digitalRead(int pin) const
    {
        auto it = m_pinStates.find(pin);
        if (it != m_pinStates.end())
        {
            std::string valueStr = (it->second == PinValue::HIGH) ? "HIGH" : "LOW";
            std::cout << "StubGpioController: Read pin " << pin << " = " << valueStr << std::endl;
            return it->second;
        }

        // Default to LOW if not previously set
        std::cout << "StubGpioController: Read pin " << pin << " = LOW (default)" << std::endl;
        return PinValue::LOW;
    }

    bool StubGpioController::isInitialized() const
    {
        return true; // Stub is always "initialized"
    }

} // namespace Wallbox