#ifndef CP_SIGNAL_READER_FACTORY_H
#define CP_SIGNAL_READER_FACTORY_H

#include "ICpSignalReader.h"
#include "IGpioController.h"
#include "INetworkCommunicator.h"
#include <memory>
#include <string>

namespace Wallbox
{

    /**
     * @brief Factory for creating CP signal readers
     *
     * Creates appropriate CP reader based on mode (development/production).
     *
     * Design Patterns:
     * - Factory Pattern: Creates different CP reader implementations
     * - Strategy Pattern: Selects strategy based on configuration
     *
     * SOLID Principles:
     * - Single Responsibility: Only creates CP readers
     * - Open/Closed: Easy to add new reader types
     * - Dependency Inversion: Returns interface, not concrete class
     */
    class CpSignalReaderFactory
    {
    public:
        /**
         * @brief Create CP signal reader for development mode (simulator)
         * @param network Network communicator for UDP
         * @return CP signal reader instance
         */
        static std::unique_ptr<ICpSignalReader> createSimulatorReader(
            std::shared_ptr<INetworkCommunicator> network);

        /**
         * @brief Create CP signal reader for production mode (hardware)
         * @param gpio GPIO controller for pin access
         * @param cpPin Pin number for CP signal
         * @return CP signal reader instance
         */
        static std::unique_ptr<ICpSignalReader> createHardwareReader(
            std::shared_ptr<IGpioController> gpio,
            int cpPin);

        /**
         * @brief Create CP signal reader based on mode string
         * @param mode "development" or "production"
         * @param gpio GPIO controller (used if production)
         * @param network Network communicator (used if development)
         * @param cpPin CP pin number (used if production)
         * @return CP signal reader instance
         */
        static std::unique_ptr<ICpSignalReader> create(
            const std::string &mode,
            std::shared_ptr<IGpioController> gpio,
            std::shared_ptr<INetworkCommunicator> network,
            int cpPin);
    };

} // namespace Wallbox

#endif // CP_SIGNAL_READER_FACTORY_H
