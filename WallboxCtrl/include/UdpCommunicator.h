/**
 * @file UdpCommunicator.h
 * @brief UDP Network Communication Implementation
 *
 * Concrete implementation of INetworkCommunicator using UDP sockets.
 */

#ifndef UDP_COMMUNICATOR_H
#define UDP_COMMUNICATOR_H

#include "INetworkCommunicator.h"
#include <string>
#include <thread>
#include <atomic>
#include <arpa/inet.h>
#include <sys/socket.h>

namespace Wallbox
{

    /**
     * @brief UDP implementation of network communicator
     */
    class UdpCommunicator : public INetworkCommunicator
    {
    public:
        /**
         * @brief Construct UDP communicator
         * @param listenPort Local port to listen on
         * @param sendPort Remote port to send to
         * @param sendAddress Destination IP address
         */
        UdpCommunicator(int listenPort, int sendPort, const std::string &sendAddress);
        ~UdpCommunicator() override;

        // INetworkCommunicator interface implementation
        bool connect() override;
        void disconnect() override;
        bool send(const std::vector<uint8_t> &data) override;
        void startReceiving(MessageCallback callback) override;
        void stopReceiving() override;
        bool isConnected() const override;

    private:
        int m_listenPort;
        int m_sendPort;
        std::string m_sendAddress;
        int m_socketFd;
        bool m_running;
        MessageCallback m_messageCallback;
        std::thread m_receiveThread;

        void receiveLoop();
    };

} // namespace Wallbox

#endif // UDP_COMMUNICATOR_H
