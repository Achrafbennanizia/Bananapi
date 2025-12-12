#include "SimpleWallboxController.h"
#include "GpioFactory.h"
#include "Configuration.h"
#include <csignal>
#include <memory>
#include <iostream>
#include <string>
#include <atomic>

using namespace Wallbox;

// Global controller instance for signal handling
static std::unique_ptr<SimpleWallboxController> g_controller;
static std::atomic<bool> g_running(true);

/**
 * @brief Signal handler for graceful shutdown
 */
void signalHandler(int signal)
{
    (void)signal;
    std::cout << "\nShutting down..." << std::endl;
    g_running = false;
}

/**
 * @brief Main entry point for Wallbox Controller v4.0
 *
 * Version 4.0 Features:
 * - Simplified - ONLY relay control
 * - C++14 standard (minimum for std::make_unique)
 * - Default relay pin: GPIO 21
 * - No UDP, no network, no state machine
 * - Direct GPIO control only
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    std::cout << "==================================================" << std::endl;
    std::cout << "  Wallbox Controller v4.0 - Simple Relay Control" << std::endl;
    std::cout << "==================================================" << std::endl;

    // Register signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // Get configuration
    auto &config = Configuration::getInstance();
    config.loadFromEnvironment();

    // Set relay pin to 21
    config.setRelayPin(21);

    std::cout << "Mode: " << config.getModeString() << std::endl;
    std::cout << "GPIO Type: " << config.getGpioType() << std::endl;
    std::cout << "Relay Pin: " << config.getRelayPin() << std::endl;
    std::cout << std::endl;

    try
    {
        // Create GPIO controller
        auto gpio = GpioFactory::create(config.getGpioType());

        // Create simple controller
        g_controller = std::make_unique<SimpleWallboxController>(
            std::move(gpio),
            config.getRelayPin());

        // Initialize
        if (!g_controller->initialize())
        {
            std::cerr << "Failed to initialize controller" << std::endl;
            return 1;
        }

        std::cout << "\n=== Wallbox Controller Ready ===" << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  on    - Enable relay" << std::endl;
        std::cout << "  off   - Disable relay" << std::endl;
        std::cout << "  status - Show relay status" << std::endl;
        std::cout << "  quit   - Exit" << std::endl;
        std::cout << "================================\n"
                  << std::endl;

        // Simple command loop
        std::string command;
        while (g_running)
        {
            std::cout << "> ";
            if (!std::getline(std::cin, command))
            {
                break;
            }

            if (command == "on")
            {
                g_controller->enableRelay();
            }
            else if (command == "off")
            {
                g_controller->disableRelay();
            }
            else if (command == "status")
            {
                std::cout << "Relay status: " << (g_controller->isRelayEnabled() ? "ON" : "OFF") << std::endl;
            }
            else if (command == "quit" || command == "exit")
            {
                break;
            }
            else if (!command.empty())
            {
                std::cout << "Unknown command. Type: on, off, status, or quit" << std::endl;
            }
        }

        // Shutdown
        g_controller->shutdown();
        std::cout << "Goodbye!" << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
