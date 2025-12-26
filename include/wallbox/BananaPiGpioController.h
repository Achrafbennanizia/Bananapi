#ifndef BANANA_PI_GPIO_CONTROLLER_H
#define BANANA_PI_GPIO_CONTROLLER_H

#include "IGpioController.h"
#include <string>

namespace Wallbox
{

    /**
     * @brief GPIO controller for Banana Pi hardware
     *
     * Uses direct sysfs GPIO access for Banana Pi hardware.
     * This implementation works on real Banana Pi boards.
     *
     * Design Pattern: Strategy Pattern
     * SOLID Principle: Liskov Substitution Principle
     */
    class BananaPiGpioController : public IGpioController
    {
    public:
        BananaPiGpioController();
        ~BananaPiGpioController() override;

        bool initialize() override;
        void shutdown() override;

        bool setPinMode(int pin, PinMode mode) override;
        bool digitalWrite(int pin, PinValue value) override;
        PinValue digitalRead(int pin) const override;
        bool isInitialized() const override;

    private:
        bool exportPin(int pin);
        bool unexportPin(int pin);
        bool setDirection(int pin, const std::string &direction);
        bool setValue(int pin, int value) const;
        int getValue(int pin) const;

        bool m_initialized;
        static constexpr const char *GPIO_PATH = "/sys/class/gpio";
    };

} // namespace Wallbox

#endif // BANANA_PI_GPIO_CONTROLLER_H
