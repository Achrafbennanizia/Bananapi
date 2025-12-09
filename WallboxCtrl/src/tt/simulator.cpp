#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <cstring>
#include <csignal>
#include <string>
#include <iomanip>
#include <sstream>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../../LibPubWallbox/IsoStackCtrlProtocol.h"

using namespace Iso15118;

// Log file
static std::ofstream g_logFile;

// Helper function to get timestamp
std::string get_timestamp()
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

// Unified logging function - writes only to file
void log_msg(const std::string &level, const std::string &message)
{
    std::string log_line = "[" + get_timestamp() + "] [" + level + "] " + message;

    // Write to log file only
    if (g_logFile.is_open())
    {
        g_logFile << log_line << std::endl;
        g_logFile.flush();
    }
}

// ---------- Konfiguration ----------
static constexpr int UDP_IN_PORT = 50011;  // WallboxCtrl -> Simulator
static constexpr int UDP_OUT_PORT = 50010; // Simulator -> WallboxCtrl
static const char *WALLBOX_IP = "127.0.0.1";

// Globale Zustände
static bool g_run = true;
static bool g_mainContactorCmd = false; // Kommando für Main Contactor
static enIsoChargingState g_chargingState = enIsoChargingState::idle;
static bool g_lastEnableState = false;   // Letzter enable-Zustand
static uint16_t g_lastCurrentDemand = 0; // Letzter currentDemand
static bool g_lastRelayState = true;     // Letzter mainContactor-Zustand

// ---------- Signal-Handler ----------
void on_sigint(int)
{
    g_run = false;
}

// ---------- UDP Setup ----------
int make_udp_in_sock()
{
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        perror("socket in");
        std::exit(1);
    }
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(UDP_IN_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(s, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        std::exit(1);
    }
    return s;
}

int make_udp_out_sock(sockaddr_in &dst)
{
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        perror("socket out");
        std::exit(1);
    }
    std::memset(&dst, 0, sizeof(dst));
    dst.sin_family = AF_INET;
    dst.sin_port = htons(UDP_OUT_PORT);
    inet_pton(AF_INET, WALLBOX_IP, &dst.sin_addr);
    return s;
}

// ---------- Empfangen: stSeIsoStackCmd ----------
void recv_cmd(int sock)
{
    uint8_t buffer[256]{};
    sockaddr_in src{};
    socklen_t slen = sizeof(src);

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sock, &fds);
    timeval tv{};
    tv.tv_sec = 0;
    tv.tv_usec = 50000; // 50 ms

    int ret = select(sock + 1, &fds, nullptr, nullptr, &tv);
    if (ret <= 0)
    {
        return; // kein Paket
    }

    ssize_t n = recvfrom(sock, buffer, sizeof(buffer), 0,
                         (sockaddr *)&src, &slen);
    if (n < (ssize_t)sizeof(stSeIsoStackCmd))
    {
        return; // Zu klein
    }

    stSeIsoStackCmd cmd{};
    std::memcpy(&cmd, buffer, sizeof(cmd));

    if (cmd.isoStackCmd.msgType != enIsoStackMsgType::SeCtrlCmd &&
        cmd.isoStackCmd.msgType != enIsoStackMsgType::CtrlCmd)
    {
        return; // falscher Typ
    }

    // Nur ausgeben wenn sich etwas geändert hat
    bool enableState = (cmd.isoStackCmd.enable != 0);
    uint16_t currentDemand = cmd.isoStackCmd.currentDemand;

    g_lastRelayState = cmd.seHardwareState.mainContactor;

    // Nur ausgeben wenn sich etwas geändert hat
    if (enableState != g_lastEnableState || currentDemand != g_lastCurrentDemand)
    {
        std::stringstream ss;
        ss << "[RX] enable=" << (enableState ? "true" : "false")
           << " currentDemand=" << currentDemand << " A/10";
        log_msg("RX", ss.str());
        std::cout << "> " << std::flush;

        g_lastEnableState = enableState;
        g_lastCurrentDemand = currentDemand;
    }
}

// ---------- Senden: stSeIsoStackState ----------
void send_state(int sock, const sockaddr_in &dst)
{
    stSeIsoStackState state{};
    state.isoStackState.clear();
    state.seHardwareCmd.clear();

    // IsoStack State setzen
    state.isoStackState.msgVersion = 0;
    state.isoStackState.msgType = enIsoStackMsgType::SeCtrlState;
    state.isoStackState.state = g_chargingState;
    state.isoStackState.supplyPhases = enSupplyPhases::ac3;
    state.isoStackState.current = 160;  // 16.0 A
    state.isoStackState.voltage = 2300; // 230.0 V

    // Hardware Command setzen
    state.seHardwareCmd.mainContactor = g_mainContactorCmd ? 1 : 0;
    state.seHardwareCmd.sourceEnable = g_mainContactorCmd ? 1 : 0;
    state.seHardwareCmd.sourceVoltage = 2300;
    state.seHardwareCmd.sourceCurrent = 160;

    ssize_t n = sendto(sock,
                       &state,
                       sizeof(state),
                       0,
                       (const sockaddr *)&dst,
                       sizeof(dst));
    if (n < 0)
    {
        perror("sendto");
    }
}

// ---------- Benutzer-Interface ----------
void print_help()
{
    std::cout << "\n=== ISO 15118 Stack Simulator ===\n";
    std::cout << "Commands:\n";
    std::cout << "  on      - Turn main contactor ON\n";
    std::cout << "  off     - Turn main contactor OFF\n";
    std::cout << "  idle    - Set charging state to IDLE\n";
    std::cout << "  ready   - Set charging state to READY\n";
    std::cout << "  charge  - Set charging state to CHARGING\n";
    std::cout << "  stop    - Set charging state to STOP\n";
    std::cout << "  status  - Show current status\n";
    std::cout << "  help    - Show this help\n";
    std::cout << "  quit    - Exit simulator\n";
    std::cout << "================================\n\n";
}

void print_status()
{
    std::cout << "\n--- Current Status ---\n";
    std::cout << "Main Contactor: " << (g_mainContactorCmd ? "ON" : "OFF") << "\n";
    std::cout << "Charging State: " << enIsoChargingState_toString(g_chargingState) << "\n";
    std::cout << "---------------------\n\n";
}

bool ensure_wallbox_enabled(const std::string &action)
{
    if (!g_lastEnableState)
    {
        std::stringstream ss;
        ss << "Cannot execute '" << action << "': Wallbox enable=false (charging disabled)";
        log_msg("WARN", ss.str());
        return false;
    }
    return true;
}

void process_command(const std::string &cmd)
{
    if (cmd == "on")
    {
        g_mainContactorCmd = true;
        log_msg("CMD", "Main contactor ON");
    }
    else if (cmd == "off")
    {
        g_mainContactorCmd = false;
        log_msg("CMD", "Main contactor OFF");
    }
    else if (cmd == "idle")
    {
        if (!ensure_wallbox_enabled("idle"))
        {
            return;
        }
        if (!g_lastRelayState)
        {
            log_msg("WARN", "Cannot enter IDLE state: Main contactor is OFF");
            return;
        }
        g_chargingState = enIsoChargingState::idle;
        log_msg("CMD", "State: IDLE");
    }
    else if (cmd == "ready")
    {
        if (!ensure_wallbox_enabled("ready"))
        {
            return;
        }
        if (!g_lastRelayState)
        {
            log_msg("WARN", "Cannot enter READY state: Main contactor is OFF");
            return;
        }
        g_chargingState = enIsoChargingState::ready;
        log_msg("CMD", "State: READY");
    }
    else if (cmd == "charge")
    {
        if (!ensure_wallbox_enabled("charge"))
        {
            return;
        }
        if (!g_lastRelayState)
        {
            log_msg("WARN", "Cannot enter CHARGING state: Main contactor is OFF");
            return;
        }
        g_chargingState = enIsoChargingState::charging;
        log_msg("CMD", "State: CHARGING");
    }
    else if (cmd == "stop")
    {
        if (!ensure_wallbox_enabled("stop"))
        {
            return;
        }
        if (!g_lastRelayState)
        {
            log_msg("WARN", "Cannot enter STOP state: Main contactor is OFF");
            return;
        }
        g_chargingState = enIsoChargingState::stop;
        log_msg("CMD", "State: STOP");
    }
    else if (cmd == "status")
    {
        print_status();
    }
    else if (cmd == "help")
    {
        print_help();
    }
    else if (cmd == "quit" || cmd == "exit")
    {
        g_run = false;
    }
    else if (!cmd.empty())
    {
        std::cout << "Unknown command. Type 'help' for available commands.\n";
    }
}

// ---------- Hauptprogramm ----------
int main()
{
    // Open log file
    g_logFile.open("/tmp/wallbox_simulator.log", std::ios::out | std::ios::app);
    if (!g_logFile.is_open())
    {
        std::cerr << "Warning: Could not open log file /tmp/wallbox_simulator.log" << std::endl;
    }

    std::signal(SIGINT, on_sigint);

    log_msg("INFO", "ISO 15118 Stack Simulator starting...");
    log_msg("INFO", std::string("Sending to: ") + WALLBOX_IP + ":" + std::to_string(UDP_OUT_PORT));
    log_msg("INFO", std::string("Listening on: *:") + std::to_string(UDP_IN_PORT));

    std::cout << "ISO 15118 Stack Simulator starting...\n";
    std::cout << "Sending to: " << WALLBOX_IP << ":" << UDP_OUT_PORT << "\n";
    std::cout << "Listening on: *:" << UDP_IN_PORT << "\n";
    std::cout << "Log file: /tmp/wallbox_simulator.log\n";

    int sockIn = make_udp_in_sock();
    sockaddr_in dst{};
    int sockOut = make_udp_out_sock(dst);

    print_help();

    // Stdin non-blocking setup
    std::cout << "> " << std::flush;

    std::string input_buffer;
    auto last_send = std::chrono::steady_clock::now();

    while (g_run)
    {
        // 1) Empfange Antworten vom WallboxCtrl
        recv_cmd(sockIn);

        // 2) Sende State alle 100ms
        auto now = std::chrono::steady_clock::now();
        if (now - last_send >= std::chrono::milliseconds(100))
        {
            send_state(sockOut, dst);
            last_send = now;
        }

        // 3) Prüfe auf Benutzereingaben (non-blocking)
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        timeval tv{};
        tv.tv_sec = 0;
        tv.tv_usec = 10000; // 10 ms

        int ret = select(STDIN_FILENO + 1, &read_fds, nullptr, nullptr, &tv);
        if (ret > 0 && FD_ISSET(STDIN_FILENO, &read_fds))
        {
            std::getline(std::cin, input_buffer);
            process_command(input_buffer);
            if (g_run)
            {
                std::cout << "> " << std::flush;
            }
        }
    }

    // Aufräumen
    close(sockIn);
    close(sockOut);

    log_msg("INFO", "Simulator stopped");
    std::cout << "\nSimulator stopped.\n";

    // Close log file
    if (g_logFile.is_open())
    {
        g_logFile.close();
    }

    return 0;
}
