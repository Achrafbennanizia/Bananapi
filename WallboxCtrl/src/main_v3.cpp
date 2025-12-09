#include "Application.h"
#include <csignal>
#include <memory>
#include <iostream>

using namespace Wallbox;

// Global application instance for signal handling
static std::unique_ptr<Application> g_application;

/**
 * @brief Signal handler for graceful shutdown
 *
 * Handles SIGINT (Ctrl+C) and SIGTERM for clean application exit.
 */
void signalHandler(int signal)
{
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
    if (g_application)
    {
        g_application->requestShutdown();
    }
}

/**
 * @brief Main entry point
 *
 * Demonstrates clean separation of concerns:
 * - Signal handling
 * - Application lifecycle
 * - Error handling
 *
 * The entire application logic is encapsulated in the Application class,
 * making this main function extremely simple and maintainable.
 */
int main(int argc, char *argv[])
{
    (void)argc; // Suppress unused warning
    (void)argv;

    // Register signal handlers for graceful shutdown
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    try
    {
        // Create application
        g_application = std::make_unique<Application>();

        // Initialize
        if (!g_application->initialize())
        {
            std::cerr << "Failed to initialize application" << std::endl;
            return 1;
        }

        // Run main loop
        g_application->run();

        // Graceful shutdown
        g_application->shutdown();

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 1;
    }
}
