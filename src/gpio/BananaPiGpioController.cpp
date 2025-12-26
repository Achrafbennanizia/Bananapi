#include "BananaPiGpioController.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <sys/stat.h>

namespace Wallbox
{

    BananaPiGpioController::BananaPiGpioController()
        : m_initialized(false)
    {
        std::cout << "[BananaPi GPIO] Using real hardware GPIO via sysfs" << std::endl;
    }

    BananaPiGpioController::~BananaPiGpioController()
    {
        shutdown();
    }

    bool BananaPiGpioController::initialize()
    {
        std::cout << "[BananaPi GPIO] Initializing hardware GPIO..." << std::endl;

        // Check if GPIO sysfs is available
        struct stat info;
        if (stat(GPIO_PATH, &info) != 0)
        {
            std::cerr << "[BananaPi GPIO] Error: GPIO sysfs not available at " << GPIO_PATH << std::endl;
            return false;
        }

        m_initialized = true;
        std::cout << "[BananaPi GPIO] Hardware GPIO initialized successfully" << std::endl;
        return true;
    }

    void BananaPiGpioController::shutdown()
    {
        if (m_initialized)
        {
            std::cout << "[BananaPi GPIO] Shutting down GPIO..." << std::endl;
            m_initialized = false;
        }
        // Note: We don't unexport pins here to allow other programs to use them
    }

    bool BananaPiGpioController::isInitialized() const
    {
        return m_initialized;
    }

    bool BananaPiGpioController::setPinMode(int pin, PinMode mode)
    {
        std::cout << "[BananaPi GPIO] Setting pin " << pin << " to "
                  << (mode == PinMode::OUTPUT ? "OUTPUT" : "INPUT") << std::endl;

        // Export pin if not already exported
        if (!exportPin(pin))
        {
            std::cerr << "[BananaPi GPIO] Failed to export pin " << pin << std::endl;
            return false;
        }

        // Set direction
        std::string direction = (mode == PinMode::OUTPUT) ? "out" : "in";
        if (!setDirection(pin, direction))
        {
            std::cerr << "[BananaPi GPIO] Failed to set direction for pin " << pin << std::endl;
            return false;
        }

        return true;
    }

    bool BananaPiGpioController::digitalWrite(int pin, PinValue value)
    {
        int val = (value == PinValue::HIGH) ? 1 : 0;

        if (!setValue(pin, val))
        {
            std::cerr << "[BananaPi GPIO] Failed to write to pin " << pin << std::endl;
            return false;
        }

        return true;
    }

    PinValue BananaPiGpioController::digitalRead(int pin) const
    {
        int value = getValue(pin);
        return (value == 1) ? PinValue::HIGH : PinValue::LOW;
    }

    bool BananaPiGpioController::exportPin(int pin)
    {
        // Check if pin is already exported
        std::ostringstream pinPath;
        pinPath << GPIO_PATH << "/gpio" << pin;

        struct stat info;
        if (stat(pinPath.str().c_str(), &info) == 0)
        {
            // Pin already exported
            return true;
        }

        // Export the pin
        std::ostringstream exportPath;
        exportPath << GPIO_PATH << "/export";

        std::ofstream exportFile(exportPath.str());
        if (!exportFile.is_open())
        {
            std::cerr << "[BananaPi GPIO] Cannot open export file: " << exportPath.str() << std::endl;
            return false;
        }

        exportFile << pin;
        exportFile.close();

        // Wait for the pin to be available
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Verify export
        if (stat(pinPath.str().c_str(), &info) != 0)
        {
            std::cerr << "[BananaPi GPIO] Pin " << pin << " export failed" << std::endl;
            return false;
        }

        return true;
    }

    bool BananaPiGpioController::unexportPin(int pin)
    {
        std::ostringstream unexportPath;
        unexportPath << GPIO_PATH << "/unexport";

        std::ofstream unexportFile(unexportPath.str());
        if (!unexportFile.is_open())
        {
            return false;
        }

        unexportFile << pin;
        unexportFile.close();

        return true;
    }

    bool BananaPiGpioController::setDirection(int pin, const std::string &direction)
    {
        std::ostringstream directionPath;
        directionPath << GPIO_PATH << "/gpio" << pin << "/direction";

        std::ofstream directionFile(directionPath.str());
        if (!directionFile.is_open())
        {
            std::cerr << "[BananaPi GPIO] Cannot open direction file: " << directionPath.str() << std::endl;
            return false;
        }

        directionFile << direction;
        directionFile.close();

        return true;
    }

    bool BananaPiGpioController::setValue(int pin, int value) const
    {
        std::ostringstream valuePath;
        valuePath << GPIO_PATH << "/gpio" << pin << "/value";

        std::ofstream valueFile(valuePath.str());
        if (!valueFile.is_open())
        {
            std::cerr << "[BananaPi GPIO] Cannot open value file: " << valuePath.str() << std::endl;
            return false;
        }

        valueFile << value;
        valueFile.close();

        return true;
    }

    int BananaPiGpioController::getValue(int pin) const
    {
        std::ostringstream valuePath;
        valuePath << GPIO_PATH << "/gpio" << pin << "/value";

        std::ifstream valueFile(valuePath.str());
        if (!valueFile.is_open())
        {
            std::cerr << "[BananaPi GPIO] Cannot open value file: " << valuePath.str() << std::endl;
            return 0;
        }

        int value;
        valueFile >> value;
        valueFile.close();

        return value;
    }

} // namespace Wallbox
