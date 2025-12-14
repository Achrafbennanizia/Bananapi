/**
 * @file CpSignalReaderFactory.cpp
 * @brief Implementation of factory for creating CP signal readers
 *
 * Factory Pattern implementation for creating appropriate CP signal reader
 * based on operating mode (simulator vs hardware).
 * Complies with SOLID principles: SRP, OCP, DIP.
 */

#include "CpSignalReaderFactory.h"
#include "SimulatorCpSignalReader.h"
#include "HardwareCpSignalReader.h"
#include <stdexcept>
#include <iostream>

namespace Wallbox
{

    /**
     * Create simulator-based CP signal reader
     * Factory Method for simulator mode
     *
     * @param networkCommunicator Network interface for UDP communication
     * @return Unique pointer to CP signal reader interface
     *
     * Design Pattern: Factory Method Pattern
     * SOLID: Dependency Inversion Principle (returns interface)
     */
    std::unique_ptr<ICpSignalReader> CpSignalReaderFactory::createSimulatorReader(
        std::shared_ptr<INetworkCommunicator> networkCommunicator)
    {
        if (!networkCommunicator)
        {
            throw std::invalid_argument("Network communicator cannot be null for simulator mode");
        }

        std::cout << "[CpSignalReaderFactory] Creating simulator CP reader" << std::endl;
        return std::make_unique<SimulatorCpSignalReader>(networkCommunicator);
    }

    /**
     * Create hardware-based CP signal reader
     * Factory Method for production mode
     *
     * @param gpioController GPIO interface for pin reading
     * @param cpPin GPIO pin number for CP signal
     * @return Unique pointer to CP signal reader interface
     *
     * Design Pattern: Factory Method Pattern
     * SOLID: Dependency Inversion Principle (returns interface)
     */
    std::unique_ptr<ICpSignalReader> CpSignalReaderFactory::createHardwareReader(
        std::shared_ptr<IGpioController> gpioController,
        int cpPin)
    {
        if (!gpioController)
        {
            throw std::invalid_argument("GPIO controller cannot be null for hardware mode");
        }

        if (cpPin < 0)
        {
            throw std::invalid_argument("Invalid CP pin number");
        }

        std::cout << "[CpSignalReaderFactory] Creating hardware CP reader (pin: "
                  << cpPin << ")" << std::endl;
        return std::make_unique<HardwareCpSignalReader>(gpioController, cpPin);
    }

    /**
     * Create appropriate CP signal reader based on mode
     * Smart factory method that selects correct implementation
     *
     * @param mode Operating mode ("simulator" or "hardware")
     * @param gpioController GPIO interface (required for hardware mode)
     * @param networkCommunicator Network interface (required for simulator mode)
     * @param cpPin GPIO pin number (required for hardware mode, default: 7)
     * @return Unique pointer to CP signal reader interface
     *
     * Design Pattern: Factory Method Pattern with strategy selection
     * SOLID: Open/Closed Principle - extensible for new modes
     */
    std::unique_ptr<ICpSignalReader> CpSignalReaderFactory::create(
        const std::string &mode,
        std::shared_ptr<IGpioController> gpioController,
        std::shared_ptr<INetworkCommunicator> networkCommunicator,
        int cpPin)
    {
        std::cout << "[CpSignalReaderFactory] Creating CP reader for mode: " << mode << std::endl;

        if (mode == "simulator" || mode == "sim" || mode == "development" || mode == "dev")
        {
            if (!networkCommunicator)
            {
                throw std::invalid_argument("Network communicator required for simulator mode");
            }
            return createSimulatorReader(networkCommunicator);
        }
        else if (mode == "hardware" || mode == "hw" || mode == "production" || mode == "prod")
        {
            if (!gpioController)
            {
                throw std::invalid_argument("GPIO controller required for hardware mode");
            }
            return createHardwareReader(gpioController, cpPin);
        }
        else
        {
            throw std::invalid_argument("Unknown mode: " + mode +
                                        " (supported: simulator, hardware, dev, prod)");
        }
    }

} // namespace Wallbox
