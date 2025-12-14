/**
 * @file SimulatorCpSignalReader.cpp
 * @brief Implementation of CP signal reader for simulator mode (UDP communication)
 *
 * This implementation receives CP (Control Pilot) state from the simulator via UDP.
 * Follows Strategy Pattern (concrete implementation) and Observer Pattern.
 * Complies with SOLID principles: SRP, OCP, LSP, ISP, DIP.
 */

#include "SimulatorCpSignalReader.h"
#include <iostream>
#include <cstring>
#include <stdexcept>

namespace Wallbox
{

    /**
     * Constructor with Dependency Injection
     * @param networkCommunicator Network communicator interface (injected dependency)
     *
     * Design Pattern: Dependency Injection
     * SOLID Principle: Dependency Inversion Principle (depends on interface)
     */
    SimulatorCpSignalReader::SimulatorCpSignalReader(std::shared_ptr<INetworkCommunicator> networkCommunicator)
        : m_network(networkCommunicator), m_currentState(CpState::UNKNOWN), m_initialized(false)
    {
        if (!m_network)
        {
            throw std::invalid_argument("Network communicator cannot be null");
        }
    } /**
       * Destructor - ensures proper cleanup
       */
    SimulatorCpSignalReader::~SimulatorCpSignalReader()
    {
        shutdown();
    }

    /**
     * Initialize the CP signal reader
     * Registers message handler with network communicator
     *
     * @return true if initialization successful, false otherwise
     * SOLID: Single Responsibility - only handles initialization
     */
    bool SimulatorCpSignalReader::initialize()
    {
        if (m_initialized)
        {
            std::cout << "[SimulatorCpSignalReader] Already initialized" << std::endl;
            return true;
        }

        try
        {
            // Note: Network receiving is managed by WallboxController
            // SimulatorCpSignalReader will process CP messages when handleMessage() is called

            m_initialized = true;
            m_currentState = CpState::STATE_A; // Initial state: no vehicle connected

            std::cout << "[SimulatorCpSignalReader] Initialized successfully" << std::endl;
            std::cout << "[SimulatorCpSignalReader] Waiting for CP state messages (UDP)" << std::endl;
            return true;
        }
        catch (const std::exception &e)
        {
            std::cerr << "[SimulatorCpSignalReader] Initialization failed: " << e.what() << std::endl;
            return false;
        }
    }

    /**
     * Shutdown the CP signal reader
     * Cleans up resources and stops monitoring
     *
     * SOLID: Single Responsibility - only handles cleanup
     */
    void SimulatorCpSignalReader::shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_callbacks.clear();
        m_initialized = false;
        m_currentState = CpState::UNKNOWN;

        std::cout << "[SimulatorCpSignalReader] Shutdown complete" << std::endl;
    }

    /**
     * Read current CP state
     * Thread-safe access to current state
     *
     * @return Current CP state
     * SOLID: Single Responsibility - only reads state
     */
    CpState SimulatorCpSignalReader::readCpState()
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        return m_currentState;
    }

    /**
     * Get string representation of CP state
     *
     * @param state CP state to convert
     * @return String representation
     */
    std::string SimulatorCpSignalReader::getCpStateString(CpState state) const
    {
        switch (state)
        {
        case CpState::STATE_A:
            return "STATE_A (12V - No Vehicle)";
        case CpState::STATE_B:
            return "STATE_B (9V - Vehicle Connected)";
        case CpState::STATE_C:
            return "STATE_C (6V - Ready to Charge)";
        case CpState::STATE_D:
            return "STATE_D (3V - Ventilation Required)";
        case CpState::STATE_E:
            return "STATE_E (0V - No Power)";
        case CpState::STATE_F:
            return "STATE_F (-12V - Error)";
        case CpState::UNKNOWN:
        default:
            return "UNKNOWN";
        }
    }

    /**
     * Register callback for CP state changes
     * Observer Pattern: Register observer
     *
     * @param callback Function to call when state changes
     * SOLID: Open/Closed Principle - extensible without modification
     */
    void SimulatorCpSignalReader::onStateChange(std::function<void(CpState, CpState)> callback)
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_callbacks.push_back(callback);
    }

    /**
     * Start monitoring CP signal (not needed for simulator - always active)
     * Included for interface compliance
     */
    void SimulatorCpSignalReader::startMonitoring()
    {
        // Simulator mode doesn't need explicit monitoring thread
        // Messages are received asynchronously via UDP callback
        std::cout << "[SimulatorCpSignalReader] Monitoring active (UDP-based)" << std::endl;
    }

    /**
     * Stop monitoring CP signal
     * Included for interface compliance
     */
    void SimulatorCpSignalReader::stopMonitoring()
    {
        // No explicit monitoring thread to stop in simulator mode
        std::cout << "[SimulatorCpSignalReader] Monitoring stopped" << std::endl;
    }

    /**
     * Set CP state manually (for testing)
     * Allows direct state injection during development/testing
     *
     * @param state New CP state to set
     * SOLID: Single Responsibility - only sets state
     */
    void SimulatorCpSignalReader::setCpState(CpState state)
    {
        CpState oldState;
        {
            std::lock_guard<std::mutex> lock(m_stateMutex);
            oldState = m_currentState;
            m_currentState = state;
        }

        if (oldState != state)
        {
            std::cout << "[SimulatorCpSignalReader] Manual CP state change: "
                      << getCpStateString(oldState) << " -> "
                      << getCpStateString(state) << std::endl;
            notifyStateChange(oldState, state);
        }
    }

    /**
     * Handle incoming network message from simulator
     * Observer Pattern: React to network events
     *
     * @param data Received UDP message data
     * SOLID: Single Responsibility - only handles message parsing
     */
    void SimulatorCpSignalReader::handleMessage(const std::vector<uint8_t> &data)
    {
        if (!m_initialized)
        {
            return;
        }

        // Parse CP state from message
        CpState newState = parseStateFromMessage(data);

        if (newState == CpState::UNKNOWN)
        {
            return; // Invalid message, ignore
        }

        CpState oldState;
        {
            std::lock_guard<std::mutex> lock(m_stateMutex);
            oldState = m_currentState;

            if (oldState == newState)
            {
                return; // No change, no notification needed
            }

            m_currentState = newState;
        }

        std::cout << "[SimulatorCpSignalReader] CP state change detected: "
                  << getCpStateString(oldState) << " -> "
                  << getCpStateString(newState) << std::endl;

        // Notify all registered observers
        notifyStateChange(oldState, newState);
    }

    /**
     * Parse CP state from UDP message
     * Expected message format:
     * - Byte 0: Message type (0x03 = CP state update)
     * - Byte 1: CP state value (0-6)
     *
     * @param data Raw message data
     * @return Parsed CP state
     * SOLID: Single Responsibility - only parses message format
     */
    CpState SimulatorCpSignalReader::parseStateFromMessage(const std::vector<uint8_t> &data)
    {
        if (data.size() < 2)
        {
            std::cerr << "[SimulatorCpSignalReader] Invalid message size: " << data.size() << std::endl;
            return CpState::UNKNOWN;
        }

        // Check message type (0x03 = CP state update)
        if (data[0] != 0x03)
        {
            // Not a CP state message, ignore
            return CpState::UNKNOWN;
        }

        // Parse CP state value
        uint8_t stateValue = data[1];

        switch (stateValue)
        {
        case 0:
            return CpState::STATE_A;
        case 1:
            return CpState::STATE_B;
        case 2:
            return CpState::STATE_C;
        case 3:
            return CpState::STATE_D;
        case 4:
            return CpState::STATE_E;
        case 5:
            return CpState::STATE_F;
        default:
            std::cerr << "[SimulatorCpSignalReader] Invalid CP state value: "
                      << static_cast<int>(stateValue) << std::endl;
            return CpState::UNKNOWN;
        }
    }

    /**
     * Notify all registered observers of state change
     * Observer Pattern: Notify all observers
     *
     * @param oldState Previous CP state
     * @param newState New CP state
     * SOLID: Single Responsibility - only handles notification
     */
    void SimulatorCpSignalReader::notifyStateChange(CpState oldState, CpState newState)
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);

        for (const auto &callback : m_callbacks)
        {
            try
            {
                callback(oldState, newState);
            }
            catch (const std::exception &e)
            {
                std::cerr << "[SimulatorCpSignalReader] Callback exception: " << e.what() << std::endl;
            }
        }
    }

} // namespace Wallbox
