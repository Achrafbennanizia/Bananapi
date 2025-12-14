#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <cstring>
#include <csignal>
#include <string>
#include <iomanip>
#include <sstream>
#include <cstdlib>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "IsoStackCtrlProtocol.h"

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
static int UDP_IN_PORT = 50011;  // WallboxCtrl -> Simulator
static int UDP_OUT_PORT = 50010; // Simulator -> WallboxCtrl
static std::string WALLBOX_IP = "127.0.0.1";

// Load configuration from config.json
void load_config()
{
    std::ifstream config_file("config.json");
    if (!config_file.is_open())
    {
        std::cout << "⚠️  config.json not found, using defaults" << std::endl;
        return;
    }

    std::string line, content;
    while (std::getline(config_file, line))
    {
        content += line;
    }
    config_file.close();

    // Simple JSON parsing for udp_send_address
    size_t pos = content.find("\"udp_send_address\"");
    if (pos != std::string::npos)
    {
        size_t start = content.find("\"", pos + 18);
        if (start != std::string::npos)
        {
            start++; // Move past the opening quote
            size_t end = content.find("\"", start);
            if (end != std::string::npos)
            {
                WALLBOX_IP = content.substr(start, end - start);
                std::cout << "✓ Loaded IP from config.json: " << WALLBOX_IP << std::endl;
            }
        }
    }

    // Parse UDP ports
    pos = content.find("\"udp_listen_port\"");
    if (pos != std::string::npos)
    {
        size_t start = content.find(":", pos);
        if (start != std::string::npos)
        {
            std::string num_str;
            for (size_t i = start + 1; i < content.length(); i++)
            {
                if (isdigit(content[i]))
                {
                    num_str += content[i];
                }
                else if (!num_str.empty())
                {
                    break;
                }
            }
            if (!num_str.empty())
            {
                UDP_OUT_PORT = std::stoi(num_str);
                std::cout << "✓ Loaded UDP listen port: " << UDP_OUT_PORT << std::endl;
            }
        }
    }

    pos = content.find("\"udp_send_port\"");
    if (pos != std::string::npos)
    {
        size_t start = content.find(":", pos);
        if (start != std::string::npos)
        {
            std::string num_str;
            for (size_t i = start + 1; i < content.length(); i++)
            {
                if (isdigit(content[i]))
                {
                    num_str += content[i];
                }
                else if (!num_str.empty())
                {
                    break;
                }
            }
            if (!num_str.empty())
            {
                UDP_IN_PORT = std::stoi(num_str);
                std::cout << "✓ Loaded UDP send port: " << UDP_IN_PORT << std::endl;
            }
        }
    }
}

// Globale Zustände
static bool g_run = true;
static bool g_mainContactorCmd = false; // Kommando für Main Contactor
static enIsoChargingState g_chargingState = enIsoChargingState::idle;
static enIsoChargingState g_prevChargingState = enIsoChargingState::idle; // Previous state for change detection

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
    inet_pton(AF_INET, WALLBOX_IP.c_str(), &dst.sin_addr);
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

    log_msg("UDP_RX", std::string("Received ") + std::to_string(n) + " bytes from wallbox");

    // Display wallbox feedback without storing state
    static bool prevEnableState = true;
    static bool prevRelayState = false;
    static int msgCount = 0;

    bool wallboxEnable = (cmd.isoStackCmd.enable != 0);
    bool wallboxRelay = cmd.seHardwareState.mainContactor;

    msgCount++;

    // Display and log important wallbox state changes
    if (wallboxEnable != prevEnableState)
    {
        if (!wallboxEnable)
        {
            std::cout << "\n[WALLBOX] 🔴 DISABLED" << std::endl;
            std::cout << "> " << std::flush;
        }
        else
        {
            std::cout << "\n[WALLBOX] 🟢 ENABLED" << std::endl;
            std::cout << "> " << std::flush;
        }
        log_msg("WALLBOX", std::string("Wallbox enable changed to ") + (wallboxEnable ? "ENABLED" : "DISABLED"));
        prevEnableState = wallboxEnable;
    }

    if (wallboxRelay != prevRelayState)
    {
        if (wallboxRelay)
        {
            std::cout << "\n[WALLBOX] ⚡ Contactor ON" << std::endl;
            std::cout << "> " << std::flush;
        }
        else
        {
            std::cout << "\n[WALLBOX] 🔌 Contactor OFF" << std::endl;
            std::cout << "> " << std::flush;
        }
        log_msg("WALLBOX", std::string("Main contactor changed to ") + (wallboxRelay ? "ON" : "OFF"));
        prevRelayState = wallboxRelay;
    }

    // Periodic debug logging
    if (msgCount % 100 == 0)
    {
        log_msg("DEBUG", "Received " + std::to_string(msgCount) + " messages from wallbox");
    }
}

// ---------- Senden: stSeIsoStackState ----------
void send_state(int sock, const sockaddr_in &dst)
{
    // Detect state changes and log to file only
    if (g_chargingState != g_prevChargingState)
    {
        log_msg("STATE", std::string("Transition: ") + enIsoChargingState_toString(g_prevChargingState) + " → " + enIsoChargingState_toString(g_chargingState));
        g_prevChargingState = g_chargingState;
    }

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
    state.seHardwareCmd.sourceEnable = 1; // Always send enabled (wallbox manages its own enable state)
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
        log_msg("ERROR", std::string("sendto failed: ") + strerror(errno));
    }
    else
    {
        log_msg("UDP_TX", std::string("Sent state=") + enIsoChargingState_toString(g_chargingState) +
                              ", contactor=" + (g_mainContactorCmd ? "ON" : "OFF") +
                              ", " + std::to_string(n) + " bytes");
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
    std::cout << "  getudp  - Show UDP configuration\n";
    std::cout << "  setudp <addr> <in_port> <out_port> - Change UDP config\n";
    std::cout << "  help    - Show this help\n";
    std::cout << "  quit    - Exit simulator\n";
    std::cout << "================================\n\n";
}

void print_status()
{
    std::cout << "\n--- Current Status ---\n";
    std::cout << "Main Contactor: " << (g_mainContactorCmd ? "ON" : "OFF") << "\n";
    std::cout << "Charging State: " << enIsoChargingState_toString(g_chargingState);

    // Add description
    if (g_chargingState == enIsoChargingState::idle && !g_mainContactorCmd)
    {
        std::cout << " (No vehicle connected)";
    }
    else if (g_chargingState == enIsoChargingState::idle)
    {
        std::cout << " (Vehicle plugged, no charging)";
    }
    else if (g_chargingState == enIsoChargingState::ready)
    {
        std::cout << " (Ready to charge)";
    }
    else if (g_chargingState == enIsoChargingState::charging)
    {
        std::cout << " (Power transfer active)";
    }
    else if (g_chargingState == enIsoChargingState::stop)
    {
        std::cout << " (Stopping session)";
    }
    std::cout << "\n";

    std::cout << "UDP Address: " << WALLBOX_IP << "\n";
    std::cout << "UDP In Port: " << UDP_IN_PORT << "\n";
    std::cout << "UDP Out Port: " << UDP_OUT_PORT << "\n";
    std::cout << "---------------------\n\n";
}

void process_command(const std::string &cmd)
{
    if (cmd == "on")
    {
        g_mainContactorCmd = true;
        std::cout << "✓ Main contactor ON" << std::endl;
        log_msg("CMD", "Main contactor ON");
    }
    else if (cmd == "off")
    {
        g_mainContactorCmd = false;
        std::cout << "✓ Main contactor OFF" << std::endl;
        log_msg("CMD", "Main contactor OFF");
    }
    else if (cmd == "idle")
    {
        g_chargingState = enIsoChargingState::idle;
        std::cout << "✓ State: IDLE" << std::endl;
        log_msg("CMD", "State: IDLE");
    }
    else if (cmd == "ready")
    {
        g_chargingState = enIsoChargingState::ready;
        std::cout << "✓ State: READY" << std::endl;
        log_msg("CMD", "State: READY");
    }
    else if (cmd == "charge")
    {
        g_chargingState = enIsoChargingState::charging;
        std::cout << "✓ State: CHARGING" << std::endl;
        log_msg("CMD", "State: CHARGING");
    }
    else if (cmd == "stop")
    {
        g_chargingState = enIsoChargingState::stop;
        std::cout << "✓ State: STOP" << std::endl;
        log_msg("CMD", "State: STOP");
    }
    else if (cmd == "getudp")
    {
        std::cout << "\nUDP Configuration:\n";
        std::cout << "  Target Address: " << WALLBOX_IP << "\n";
        std::cout << "  Listen Port (in): " << UDP_IN_PORT << "\n";
        std::cout << "  Send Port (out): " << UDP_OUT_PORT << "\n\n";
    }
    else if (cmd.substr(0, 7) == "setudp ")
    {
        std::istringstream iss(cmd.substr(7));
        std::string address;
        int in_port, out_port;
        if (iss >> address >> in_port >> out_port)
        {
            if (in_port > 0 && in_port < 65536 && out_port > 0 && out_port < 65536)
            {
                WALLBOX_IP = address;
                UDP_IN_PORT = in_port;
                UDP_OUT_PORT = out_port;
                std::cout << "✓ UDP configuration updated to: " << address << " " << in_port << " -> " << out_port << std::endl;
                std::cout << "  (Restart simulator to rebind ports)" << std::endl;
                log_msg("CMD", "UDP config changed: " + address + " " + std::to_string(in_port) + " -> " + std::to_string(out_port));
            }
            else
            {
                std::cout << "✗ Invalid port numbers. Use 1-65535" << std::endl;
            }
        }
        else
        {
            std::cout << "✗ Invalid format. Usage: setudp <address> <in_port> <out_port>" << std::endl;
        }
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

    // Load configuration from config.json
    load_config();

    log_msg("INFO", "ISO 15118 Stack Simulator starting...");
    log_msg("INFO", std::string("Sending to: ") + WALLBOX_IP + ":" + std::to_string(UDP_OUT_PORT));
    log_msg("INFO", std::string("Listening on: *:") + std::to_string(UDP_IN_PORT));

    std::cout << "\nISO 15118 Stack Simulator starting...\n";
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
