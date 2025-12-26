#include "UdpCommunicator.h"
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <iostream>
#include <thread>
#include <chrono>

namespace Wallbox
{

    UdpCommunicator::UdpCommunicator(int listenPort, int sendPort, const std::string &sendAddress)
        : m_listenPort(listenPort), m_sendPort(sendPort), m_sendAddress(sendAddress), m_socketFd(-1), m_running(false)
    {
    }

    UdpCommunicator::~UdpCommunicator()
    {
        disconnect();
    }

    bool UdpCommunicator::connect()
    {
        // Create UDP socket
        m_socketFd = socket(AF_INET, SOCK_DGRAM, 0);
        if (m_socketFd < 0)
        {
            std::cerr << "Failed to create UDP socket: " << strerror(errno) << std::endl;
            return false;
        }

        // Allow address reuse
        int opt = 1;
        if (setsockopt(m_socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        {
            std::cerr << "Failed to set SO_REUSEADDR: " << strerror(errno) << std::endl;
            close(m_socketFd);
            m_socketFd = -1;
            return false;
        }

        // Bind to listen port
        sockaddr_in listenAddr{};
        listenAddr.sin_family = AF_INET;
        listenAddr.sin_addr.s_addr = INADDR_ANY;
        listenAddr.sin_port = htons(m_listenPort);

        if (bind(m_socketFd, (struct sockaddr *)&listenAddr, sizeof(listenAddr)) < 0)
        {
            std::cerr << "Failed to bind to port " << m_listenPort << ": "
                      << strerror(errno) << std::endl;
            close(m_socketFd);
            m_socketFd = -1;
            return false;
        }

        // Set socket to non-blocking mode
        int flags = fcntl(m_socketFd, F_GETFL, 0);
        fcntl(m_socketFd, F_SETFL, flags | O_NONBLOCK);

        std::cout << "UDP communicator connected on port " << m_listenPort << std::endl;
        return true;
    }

    void UdpCommunicator::disconnect()
    {
        m_running = false;

        if (m_receiveThread.joinable())
        {
            m_receiveThread.join();
        }

        if (m_socketFd >= 0)
        {
            close(m_socketFd);
            m_socketFd = -1;
        }
    }

    bool UdpCommunicator::send(const std::vector<uint8_t> &data)
    {
        if (m_socketFd < 0)
        {
            std::cerr << "Cannot send: socket not connected" << std::endl;
            return false;
        }

        sockaddr_in sendAddr{};
        sendAddr.sin_family = AF_INET;
        sendAddr.sin_port = htons(m_sendPort);

        if (inet_pton(AF_INET, m_sendAddress.c_str(), &sendAddr.sin_addr) <= 0)
        {
            std::cerr << "Invalid send address: " << m_sendAddress << std::endl;
            return false;
        }

        ssize_t sent = sendto(m_socketFd, data.data(), data.size(), 0,
                              (struct sockaddr *)&sendAddr, sizeof(sendAddr));

        if (sent < 0)
        {
            std::cerr << "Failed to send UDP packet: " << strerror(errno) << std::endl;
            return false;
        }

        if (static_cast<size_t>(sent) != data.size())
        {
            std::cerr << "Partial send: " << sent << "/" << data.size() << " bytes" << std::endl;
            return false;
        }

        return true;
    }

    void UdpCommunicator::startReceiving(MessageCallback callback)
    {
        if (m_socketFd < 0)
        {
            throw std::runtime_error("Cannot start receiving: socket not connected");
        }

        m_messageCallback = callback;
        m_running = true;

        m_receiveThread = std::thread([this]()
                                      { receiveLoop(); });
    }

    void UdpCommunicator::stopReceiving()
    {
        m_running = false;

        if (m_receiveThread.joinable())
        {
            m_receiveThread.join();
        }
    }

    bool UdpCommunicator::isConnected() const
    {
        return m_socketFd >= 0;
    }

    void UdpCommunicator::receiveLoop()
    {
        std::vector<uint8_t> buffer(4096);

        while (m_running)
        {
            sockaddr_in senderAddr{};
            socklen_t senderLen = sizeof(senderAddr);

            ssize_t received = recvfrom(m_socketFd, buffer.data(), buffer.size(), 0,
                                        (struct sockaddr *)&senderAddr, &senderLen);

            if (received > 0)
            {
                // Copy received data to appropriately sized vector
                std::vector<uint8_t> message(buffer.begin(), buffer.begin() + received);

                // Invoke callback if set
                if (m_messageCallback)
                {
                    m_messageCallback(message);
                }
            }
            else if (received < 0)
            {
                if (errno != EAGAIN && errno != EWOULDBLOCK)
                {
                    std::cerr << "Receive error: " << strerror(errno) << std::endl;
                }
            }

            // Small delay to prevent CPU spinning
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

} // namespace Wallbox
