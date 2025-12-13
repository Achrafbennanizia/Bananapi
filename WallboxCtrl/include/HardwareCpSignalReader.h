#ifndef HARDWARE_CP_SIGNAL_READER_H
#define HARDWARE_CP_SIGNAL_READER_H

#include "ICpSignalReader.h"
#include "IGpioController.h"
#include <memory>
#include <thread>
#include <atomic>
#include <vector>

namespace Wallbox
{

    /**
     * @brief Hardware implementation of CP signal reader using GPIO pins
     *
     * Reads Control Pilot signal from physical pins using voltage divider
     * and ADC (Analog-to-Digital Converter) or digital threshold detection.
     *
     * Design Patterns:
     * - Strategy Pattern: Concrete strategy for hardware reading
     * - Dependency Injection: Receives GPIO controller
     *
     * SOLID Principles:
     * - Single Responsibility: Only reads CP from hardware
     * - Dependency Inversion: Depends on IGpioController interface
     * - Open/Closed: Extensible for different hardware configurations
     */
    class HardwareCpSignalReader : public ICpSignalReader
    {
    public:
        /**
         * @brief Constructor with GPIO controller
         * @param gpio GPIO controller for pin access
         * @param cpPin Pin number for CP signal reading
         */
        HardwareCpSignalReader(std::shared_ptr<IGpioController> gpio, int cpPin);
        ~HardwareCpSignalReader() override;

        bool initialize() override;
        void shutdown() override;
        CpState readCpState() override;
        std::string getCpStateString(CpState state) const override;
        void onStateChange(CpStateChangeCallback callback) override;
        void startMonitoring() override;
        void stopMonitoring() override;
        bool isInitialized() const override { return m_initialized; }
        bool isMonitoring() const override { return m_monitoring.load(); }

    private:
        std::shared_ptr<IGpioController> m_gpio;
        int m_cpPin;
        bool m_initialized;
        std::atomic<bool> m_monitoring;
        std::thread m_monitorThread;
        CpState m_currentState;
        std::vector<CpStateChangeCallback> m_callbacks;

        /**
         * @brief Monitor loop running in separate thread
         */
        void monitorLoop();

        /**
         * @brief Convert voltage reading to CP state
         * @param voltage Voltage in mV
         * @return Corresponding CP state
         */
        CpState voltageToState(int voltage);

        /**
         * @brief Read voltage from CP pin
         * @return Voltage in mV
         */
        int readVoltage();

        /**
         * @brief Notify all callbacks of state change
         * @param oldState Previous state
         * @param newState New state
         */
        void notifyStateChange(CpState oldState, CpState newState);
    };

} // namespace Wallbox

#endif // HARDWARE_CP_SIGNAL_READER_H
