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
 * Usage:
 *   ./wallbox_control_v4                - Start with HTTP API server only (default)
 *   ./wallbox_control_v4 --interactive  - Start with interactive terminal only
 *   ./wallbox_control_v4 --dual         - Start with both API and interactive (dual mode)
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
    // Check for mode flags and config file
    bool interactiveMode = false;
    bool dualMode = false;
    std::string configFile = "config/development.json"; // Default config

    for (int i = 1; i < argc; ++i)
    {
        std::string arg(argv[i]);
        if (arg == "--interactive" || arg == "-i")
        {
            interactiveMode = true;
        }
        else if (arg == "--dual" || arg == "-d")
        {
            dualMode = true;
            interactiveMode = false; // Dual mode will handle both
        }
        else if (arg == "production" || arg == "prod")
        {
            configFile = "config/production.json";
        }
        else if (arg == "development" || arg == "dev")
        {
            configFile = "config/development.json";
        }
        else if (arg == "test")
        {
            configFile = "config/test.json";
        }
        else if (arg.find(".json") != std::string::npos)
        {
            configFile = arg; // Custom config file path
        }
    }

    // Open log file
    g_logFile.open("/tmp/wallbox_main.log", std::ios::out | std::ios::app);
    if (!g_logFile.is_open())
    {
        std::cerr << "Warning: Could not open log file /tmp/wallbox_main.log" << std::endl;
    }
    else
    {
        logMessage("INFO", "Wallbox Controller v4.0 starting...");
        logMessage("INFO", "Config file: " + configFile);
        logMessage("INFO", interactiveMode ? "Mode: Interactive Terminal" : "Mode: HTTP API Server");
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

        // Initialize (pass mode flags and config file)
        if (!g_application->initialize(interactiveMode, dualMode, configFile))
        {
            logMessage("ERROR", "Failed to initialize application");
            std::cerr << "Failed to initialize application" << std::endl;
            if (g_logFile.is_open())
                g_logFile.close();
            return 1;
        }

        logMessage("INFO", "Application initialized successfully");

        // Run main loop based on mode
        if (dualMode)
        {
            logMessage("INFO", "Starting DUAL mode (API + Interactive)");
            std::cout << "\n=== Starting Wallbox Controller V3 (DUAL MODE) ===" << std::endl;
            std::cout << "HTTP API server: http://localhost:8080" << std::endl;
            std::cout << "Interactive Terminal: Type commands below" << std::endl;
            std::cout << "Type 'help' for commands, 'quit' to stop both.\n"
                      << std::endl;
            g_application->runDual();
        }
        else if (interactiveMode)
        {
            logMessage("INFO", "Starting interactive terminal mode");
            g_application->runInteractive();
        }
        else
        {
            logMessage("INFO", "Starting HTTP API server mode");
            g_application->run();
        }

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
