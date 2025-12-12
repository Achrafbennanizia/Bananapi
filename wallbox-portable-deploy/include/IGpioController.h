/**
 * @file IGpioController.h
 * @brief GPIO Controller Interface (Interface Segregation Principle)
 *
 * This interface defines the contract for GPIO operations,
 * allowing different implementations (real hardware, stub, mock).
 */

#ifndef IGPIO_CONTROLLER_H
#define IGPIO_CONTROLLER_H

namespace Wallbox
{

    /**
     * @brief GPIO pin mode enumeration
     */
    enum class PinMode
    {
        INPUT = 0,
        OUTPUT = 1
    };

    /**
     * @brief GPIO pin value enumeration
     */
    enum class PinValue
    {
        LOW = 0,
        HIGH = 1
    };

    /**
     * @brief Interface for GPIO control operations
     *
     * This interface follows the Dependency Inversion Principle,
     * allowing high-level modules to depend on this abstraction
     * rather than concrete implementations.
     */
    class IGpioController
    {
    public:
        virtual ~IGpioController() = default;

        /**
         * @brief Initialize the GPIO system
         * @return true if successful, false otherwise
         */
        virtual bool initialize() = 0;

        /**
         * @brief Shutdown the GPIO system
         */
        virtual void shutdown() = 0;

        /**
         * @brief Set pin mode (INPUT or OUTPUT)
         * @param pin Pin number
         * @param mode Pin mode
         * @return true if successful, false otherwise
         */
        virtual bool setPinMode(int pin, PinMode mode) = 0;

        /**
         * @brief Write digital value to pin
         * @param pin Pin number
         * @param value Pin value (HIGH or LOW)
         * @return true if successful, false otherwise
         */
        virtual bool digitalWrite(int pin, PinValue value) = 0;

        /**
         * @brief Read digital value from pin
         * @param pin Pin number
         * @return Current pin value
         */
        virtual PinValue digitalRead(int pin) const = 0;

        /**
         * @brief Check if GPIO is initialized
         * @return true if initialized
         */
        virtual bool isInitialized() const = 0;
    };

} // namespace Wallbox

#endif // IGPIO_CONTROLLER_H
