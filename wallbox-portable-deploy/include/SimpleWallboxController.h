#ifndef SIMPLE_WALLBOX_CONTROLLER_H
#define SIMPLE_WALLBOX_CONTROLLER_H

#include "IGpioController.h"
#include <memory>

namespace Wallbox
{
    /**
     * @brief Simple wallbox controller - only relay control
     *
     * Version 4.0 - Simplified to only control relay via GPIO
     * No UDP, no state machine, no network communication
     */
    class SimpleWallboxController
    {
    public:
        SimpleWallboxController(std::unique_ptr<IGpioController> gpio, int relayPin);
        ~SimpleWallboxController();

        bool initialize();
        void shutdown();

        // Simple relay control
        bool enableRelay();
        bool disableRelay();
        bool isRelayEnabled() const { return m_relayEnabled; }

    private:
        std::unique_ptr<IGpioController> m_gpio;
        int m_relayPin;
        bool m_relayEnabled;

        void setupGpio();
    };

} // namespace Wallbox

#endif // SIMPLE_WALLBOX_CONTROLLER_H
