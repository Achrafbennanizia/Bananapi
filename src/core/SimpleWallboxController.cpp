#include "SimpleWallboxController.h"
#include <iostream>

namespace Wallbox
{
    SimpleWallboxController::SimpleWallboxController(std::unique_ptr<IGpioController> gpio, int relayPin)
        : m_gpio(std::move(gpio)), m_relayPin(relayPin), m_relayEnabled(false)
    {
    }

    SimpleWallboxController::~SimpleWallboxController()
    {
        shutdown();
    }

    bool SimpleWallboxController::initialize()
    {
        std::cout << "Initializing Simple Wallbox Controller..." << std::endl;

        // Initialize GPIO
        if (!m_gpio->initialize())
        {
            std::cerr << "Failed to initialize GPIO" << std::endl;
            return false;
        }

        setupGpio();
        std::cout << "Simple Wallbox Controller initialized successfully" << std::endl;
        return true;
    }

    void SimpleWallboxController::setupGpio()
    {
        // Setup relay pin as output
        m_gpio->setPinMode(m_relayPin, PinMode::OUTPUT);
        m_gpio->digitalWrite(m_relayPin, PinValue::LOW); // Start with relay OFF
    }

    void SimpleWallboxController::shutdown()
    {
        if (m_gpio)
        {
            // Turn off relay
            m_gpio->digitalWrite(m_relayPin, PinValue::LOW);
            std::cout << "Simple Wallbox Controller shut down" << std::endl;
        }
    }

    bool SimpleWallboxController::enableRelay()
    {
        if (!m_gpio)
            return false;

        m_gpio->digitalWrite(m_relayPin, PinValue::HIGH);
        m_relayEnabled = true;
        std::cout << "Relay ENABLED on pin " << m_relayPin << std::endl;
        return true;
    }

    bool SimpleWallboxController::disableRelay()
    {
        if (!m_gpio)
            return false;

        m_gpio->digitalWrite(m_relayPin, PinValue::LOW);
        m_relayEnabled = false;
        std::cout << "Relay DISABLED on pin " << m_relayPin << std::endl;
        return true;
    }

} // namespace Wallbox
