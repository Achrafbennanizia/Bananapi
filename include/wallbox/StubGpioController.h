/**
 * @file StubGpioController.h
 * @brief Stub GPIO Controller for development/testing
 *
 * Implementation of IGpioController for non-Raspberry Pi systems.
 * Uses Strategy Pattern for different GPIO implementations.
 */

#ifndef STUB_GPIO_CONTROLLER_H
#define STUB_GPIO_CONTROLLER_H

#include "IGpioController.h"
#include <iostream>
#include <map>

namespace Wallbox
{

    /**
     * @brief Stub implementation of GPIO controller
     *
     * This class implements the IGpioController interface for systems
     * without actual GPIO hardware. It logs operations to console.
     */
    class StubGpioController : public IGpioController
    {
    public:
        StubGpioController();
        ~StubGpioController() override;

        // IGpioController interface implementation
        bool initialize() override;
        void shutdown() override;
        bool setPinMode(int pin, PinMode mode) override;
        bool digitalWrite(int pin, PinValue value) override;
        PinValue digitalRead(int pin) const override;
        bool isInitialized() const override;

    private:
        std::map<int, PinValue> m_pinStates;
    };

} // namespace Wallbox

#endif // STUB_GPIO_CONTROLLER_H
