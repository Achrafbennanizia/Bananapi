#ifndef SIMULATOR_CP_SIGNAL_READER_H
#define SIMULATOR_CP_SIGNAL_READER_H

#include "ICpSignalReader.h"
#include "INetworkCommunicator.h"
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>

namespace Wallbox
{

    /**
     * @brief Simulator implementation of CP signal reader via UDP
     *
     * Receives CP signal state from external simulator over UDP network.
     * Used in development mode for testing without physical hardware.
     *
     * Design Patterns:
     * - Strategy Pattern: Concrete strategy for simulator reading
     * - Dependency Injection: Receives network communicator
     * - Observer Pattern: Callbacks for state changes
     *
     * SOLID Principles:
     * - Single Responsibility: Only reads CP from simulator
     * - Dependency Inversion: Depends on INetworkCommunicator interface
     * - Open/Closed: Can be extended without modification
     */
    class SimulatorCpSignalReader : public ICpSignalReader
    {
    public:
        /**
         * @brief Constructor with network communicator
         * @param network Network communicator for UDP messages
         */
        explicit SimulatorCpSignalReader(std::shared_ptr<INetworkCommunicator> network);
        ~SimulatorCpSignalReader() override;

        bool initialize() override;
        void shutdown() override;
        CpState readCpState() override;
        std::string getCpStateString(CpState state) const override;
        void onStateChange(CpStateChangeCallback callback) override;
        void startMonitoring() override;
        void stopMonitoring() override;
        bool isInitialized() const override { return m_initialized; }
        bool isMonitoring() const override { return m_monitoring.load(); }

        /**
         * @brief Set CP state manually (for testing)
         * @param state CP state to set
         */
        void setCpState(CpState state);

    private:
        std::shared_ptr<INetworkCommunicator> m_network;
        bool m_initialized;
        std::atomic<bool> m_monitoring;
        CpState m_currentState;
        std::mutex m_stateMutex;
        std::vector<CpStateChangeCallback> m_callbacks;

        /**
         * @brief Handle incoming UDP message from simulator
         * @param data Message data
         */
        void handleMessage(const std::vector<uint8_t> &data);

        /**
         * @brief Parse CP state from message
         * @param data Message data
         * @return Parsed CP state
         */
        CpState parseStateFromMessage(const std::vector<uint8_t> &data);

        /**
         * @brief Notify all callbacks of state change
         * @param oldState Previous state
         * @param newState New state
         */
        void notifyStateChange(CpState oldState, CpState newState);
    };

} // namespace Wallbox

#endif // SIMULATOR_CP_SIGNAL_READER_H
