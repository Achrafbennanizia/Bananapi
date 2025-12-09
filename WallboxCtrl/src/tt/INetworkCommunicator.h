/**
 * @file INetworkCommunicator.h
 * @brief Network Communication Interface
 *
 * Defines the contract for network communication operations.
 * Follows Interface Segregation Principle.
 */

#ifndef INETWORK_COMMUNICATOR_H
#define INETWORK_COMMUNICATOR_H

#include <functional>
#include <vector>
#include <cstdint>

namespace Wallbox
{

    /**
     * @brief Interface for network communication
     *
     * This interface allows for different communication implementations
     * (UDP, TCP, mock, etc.) following the Dependency Inversion Principle.
     */
    class INetworkCommunicator
    {
    public:
        using MessageCallback = std::function<void(const std::vector<uint8_t> &)>;

        virtual ~INetworkCommunicator() = default;

        /**
         * @brief Connect to the network
         * @return true if successful, false otherwise
         */
        virtual bool connect() = 0;

        /**
         * @brief Disconnect from the network
         */
        virtual void disconnect() = 0;

        /**
         * @brief Send data over the network
         * @param data Data to send
         * @return true if sent successfully, false otherwise
         */
        virtual bool send(const std::vector<uint8_t> &data) = 0;

        /**
         * @brief Start receiving messages (non-blocking)
         * @param callback Function to call when message is received
         */
        virtual void startReceiving(MessageCallback callback) = 0;

        /**
         * @brief Stop receiving messages
         */
        virtual void stopReceiving() = 0;

        /**
         * @brief Check if connected
         * @return true if connected
         */
        virtual bool isConnected() const = 0;
    };

} // namespace Wallbox

#endif // INETWORK_COMMUNICATOR_H
