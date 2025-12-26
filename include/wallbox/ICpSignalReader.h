#ifndef ICP_SIGNAL_READER_H
#define ICP_SIGNAL_READER_H

#include <cstdint>
#include <functional>
#include <string>

namespace Wallbox
{

    /**
     * @brief Control Pilot (CP) signal states according to IEC 61851-1
     *
     * The CP signal indicates the vehicle connection state and charging capability.
     */
    enum class CpState : uint8_t
    {
        STATE_A = 0, ///< No vehicle connected (12V)
        STATE_B = 1, ///< Vehicle connected, not ready to charge (9V)
        STATE_C = 2, ///< Vehicle connected, ready to charge (6V)
        STATE_D = 3, ///< Vehicle connected, ventilation required (3V)
        STATE_E = 4, ///< No power, circuit open (0V)
        STATE_F = 5, ///< Error state, EVSE malfunction (-12V)
        UNKNOWN = 6  ///< Cannot determine state
    };

    /**
     * @brief Callback for CP state changes
     * @param oldState Previous CP state
     * @param newState New CP state
     */
    using CpStateChangeCallback = std::function<void(CpState oldState, CpState newState)>;

    /**
     * @brief Interface for reading Control Pilot (CP) signals
     *
     * Design Patterns:
     * - Strategy Pattern: Different implementations for simulator vs. hardware
     * - Observer Pattern: Callbacks for state changes
     *
     * SOLID Principles:
     * - Interface Segregation: Focused interface for CP signal reading
     * - Dependency Inversion: Depend on abstraction, not concrete implementations
     * - Single Responsibility: Only handles CP signal reading
     */
    class ICpSignalReader
    {
    public:
        virtual ~ICpSignalReader() = default;

        /**
         * @brief Initialize the CP signal reader
         * @return true if initialization successful
         */
        virtual bool initialize() = 0;

        /**
         * @brief Shutdown the CP signal reader
         */
        virtual void shutdown() = 0;

        /**
         * @brief Read current CP state
         * @return Current CP state
         */
        virtual CpState readCpState() = 0;

        /**
         * @brief Get string representation of CP state
         * @param state CP state to convert
         * @return String representation
         */
        virtual std::string getCpStateString(CpState state) const = 0;

        /**
         * @brief Register callback for CP state changes
         * @param callback Function to call when state changes
         */
        virtual void onStateChange(CpStateChangeCallback callback) = 0;

        /**
         * @brief Start monitoring CP signal (continuous reading)
         */
        virtual void startMonitoring() = 0;

        /**
         * @brief Stop monitoring CP signal
         */
        virtual void stopMonitoring() = 0;

        /**
         * @brief Check if reader is initialized
         * @return true if initialized
         */
        virtual bool isInitialized() const = 0;

        /**
         * @brief Check if monitoring is active
         * @return true if monitoring
         */
        virtual bool isMonitoring() const = 0;
    };

} // namespace Wallbox

#endif // ICP_SIGNAL_READER_H
