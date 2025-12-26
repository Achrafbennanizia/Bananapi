#ifndef GPIO_FACTORY_H
#define GPIO_FACTORY_H

#include "IGpioController.h"
#include "StubGpioController.h"
#include "BananaPiGpioController.h"
#include <memory>
#include <string>
#include <iostream>

namespace Wallbox
{

    /**
     * @brief Factory for creating GPIO controllers
     *
     * Implements Factory Method pattern to create appropriate GPIO
     * controller based on configuration.
     *
     * Design Pattern: Factory Method
     * SOLID:
     * - Single Responsibility (creates GPIO controllers only)
     * - Open/Closed (easy to add new GPIO types)
     * - Dependency Inversion (returns interface, not concrete class)
     */
    class GpioFactory
    {
    public:
        /**
         * @brief Create GPIO controller based on type
         * @param type Controller type ("stub", "bananapi", "real")
         * @return Unique pointer to GPIO controller
         */
        static std::unique_ptr<IGpioController> create(const std::string &type)
        {
            if (type == "stub")
            {
                std::cout << "[GPIO Factory] Creating stub GPIO controller (development mode)" << std::endl;
                return std::make_unique<StubGpioController>();
            }
            else if (type == "bananapi" || type == "real")
            {
                std::cout << "[GPIO Factory] Creating Banana Pi GPIO controller (production mode)" << std::endl;
                return std::make_unique<BananaPiGpioController>();
            }

            std::cerr << "[GPIO Factory] Unknown GPIO type: " << type << ", defaulting to stub" << std::endl;
            return std::make_unique<StubGpioController>();
        }

        /**
         * @brief Create GPIO controller for development mode
         */
        static std::unique_ptr<IGpioController> createForDevelopment()
        {
            return create("stub");
        }

        /**
         * @brief Create GPIO controller for production mode
         */
        static std::unique_ptr<IGpioController> createForProduction()
        {
            return create("bananapi");
        }
    };

} // namespace Wallbox

#endif // GPIO_FACTORY_H
