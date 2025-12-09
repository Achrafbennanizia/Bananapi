#include "Application.h"
#include <csignal>
#include <memory>
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace Wallbox;

// Global application instance for signal handling
static std::unique_ptr<Application> g_application;

// Log file
static std::ofstream g_logFile;

/**
 * @brief Get formatted timestamp
 */
std::string getTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) %
              1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

/**
 * @brief Unified logging function - writes only to file
 */
void logMessage(const std::string &level, const std::string &message)
{
    std::string logLine = "[" + getTimestamp() + "] [" + level + "] " + message;

    // Write to log file only
    if (g_logFile.is_open())
    {
        g_logFile << logLine << std::endl;
        g_logFile.flush();
    }
}

/**
 * @brief Signal handler for graceful shutdown
 *
 * Handles SIGINT (Ctrl+C) and SIGTERM for clean application exit.
 */
void signalHandler(int signal)
{
    std::stringstream ss;
    ss << "Received signal " << signal << ", shutting down...";
    logMessage("INFO", ss.str());

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

    // Open log file
    g_logFile.open("/tmp/wallbox_main.log", std::ios::out | std::ios::app);
    if (!g_logFile.is_open())
    {
        std::cerr << "Warning: Could not open log file /tmp/wallbox_main.log" << std::endl;
    }
    else
    {
        logMessage("INFO", "Wallbox Controller v3.0 starting...");
        logMessage("INFO", "Log file: /tmp/wallbox_main.log");
    }

    // Register signal handlers for graceful shutdown
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    try
    {
        // Create application
        g_application = std::make_unique<Application>();
        logMessage("INFO", "Application instance created");

        // Initialize
        if (!g_application->initialize())
        {
            logMessage("ERROR", "Failed to initialize application");
            std::cerr << "Failed to initialize application" << std::endl;
            if (g_logFile.is_open())
                g_logFile.close();
            return 1;
        }

        logMessage("INFO", "Application initialized successfully");

        // Run main loop
        logMessage("INFO", "Starting main application loop");
        g_application->run();

        // Graceful shutdown
        logMessage("INFO", "Shutting down application");
        g_application->shutdown();
        logMessage("INFO", "Application shutdown complete");

        if (g_logFile.is_open())
            g_logFile.close();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::stringstream ss;
        ss << "Fatal error: " << e.what();
        logMessage("ERROR", ss.str());
        std::cerr << ss.str() << std::endl;
        if (g_logFile.is_open())
            g_logFile.close();
        return 1;
    }
    catch (...)
    {
        logMessage("ERROR", "Unknown fatal error occurred");
        std::cerr << "Unknown fatal error occurred" << std::endl;
        if (g_logFile.is_open())
            g_logFile.close();
        return 1;
    }
}
