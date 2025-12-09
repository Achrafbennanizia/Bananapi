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

void setElectricity(bool on);

// Simple HAL implementation for GPIO (stubbed for non-Raspberry Pi systems)
namespace HAL
{
    enum PinMode
    {
        INPUT = 0,
        OUTPUT = 1
    };
    enum PinValue
    {
        LOW = 0,
        HIGH = 1
    };

    void wiringPiSetup()
    {
        std::cout << "[HAL] GPIO system initialized (stub)\n";
    }

    void pinMode(int pin, PinMode mode)
    {
        std::cout << "[HAL] pinMode(" << pin << ", " << (mode == OUTPUT ? "OUTPUT" : "INPUT") << ")\n";
    }

    void digitalWrite(int pin, PinValue value)
    {
        // In a real implementation, this would control actual GPIO
        // For now, it's just a stub
        std::cout << "[HAL] digitalWrite(" << pin << ", " << (value == HIGH ? "HIGH" : "LOW") << ")\n";
    }
}

// ---------- Konfiguration ----------
static constexpr int UDP_IN_PORT = 50010;  // Simulator -> Pi
static constexpr int UDP_OUT_PORT = 50011; // Pi -> Simulator
static const char *SIM_IP = "127.0.0.1";   // ggf. anpassen
static constexpr int GPIO_CONTACTOR = 5;   // anpassen auf deinen Pin

// Failsafe
static constexpr auto WATCHDOG_TMO = std::chrono::seconds(2);
static std::chrono::steady_clock::time_point g_lastRx;

// Globale Zustände
static bool g_run = true;
static bool g_enable = true;       // Betrieb freigegeben?
static bool g_mainReq = false;     // mainContactor vom Stack
static bool g_relayState = false;  // aktuell gesetzter GPIO-Zustand
static bool g_lastMainReq = false; // Letzter mainContactor-Zustand für Änderungserkennung

// ---------- Signal-Handler ----------
void on_sigint(int)
{
    g_run = false;
}

// ---------- Benutzer-Interface ----------
void print_help()
{
    std::cout << "================================\n\n";
    std::cout << "\n=== Wallbox Control Commands ===\n";
    std::cout << "  enable    - Enable charging operation\n";
    std::cout << "  disable   - Disable charging operation\n";
    std::cout << "  status    - Show current status\n";
    std::cout << "  help      - Show this help\n";
    std::cout << "  quit      - Exit application\n";
    std::cout << "================================\n\n";
}

void print_status()
{
    std::cout << "\n--- Current Status ---\n";
    std::cout << "Relay: " << (g_relayState ? "ON" : "OFF") << "\n";
    std::cout << "Enable: " << (g_enable ? "true" : "false") << "\n";
    std::cout << "ISO MainContactor Request: " << (g_mainReq ? "true" : "false") << "\n";
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - g_lastRx).count();
    std::cout << "Last ISO RX: " << elapsed << " ms ago\n";
    std::cout << "Watchdog: " << ((now - g_lastRx) > WATCHDOG_TMO ? "TIMEOUT" : "OK") << "\n";
    std::cout << "---------------------\n\n";
}

void process_command(const std::string &cmd)
{
    if (cmd == "enable")
    {
        g_enable = true;
        log_msg("CMD", "g_enable set to true (charging allowed)");
        std::cout << "Charging enabled\n";
    }
    else if (cmd == "disable")
    {
        g_enable = false;
        log_msg("CMD", "g_enable set to false (charging blocked)");
        std::cout << "Charging disabled\n";

        // Sicherheit: Relais sofort ausschalten
        setElectricity(false);
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

// ---------- GPIO / Relais ----------
void init_gpio()
{
    HAL::wiringPiSetup();
    HAL::pinMode(GPIO_CONTACTOR, HAL::OUTPUT);
    HAL::digitalWrite(GPIO_CONTACTOR, HAL::LOW);
    g_relayState = false;
}

void setElectricity(bool on)
{
    if (on && !g_relayState)
    {
        HAL::digitalWrite(GPIO_CONTACTOR, HAL::HIGH);
        g_relayState = true;
        std::cout << "[GPIO] Relay ON\n";
        std::cout << "> " << std::flush;
    }
    else if (!on && g_relayState)
    {
        HAL::digitalWrite(GPIO_CONTACTOR, HAL::LOW);
        g_relayState = false;
        std::cout << "[GPIO] Relay OFF\n";
        std::cout << "> " << std::flush;
    }
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
    inet_pton(AF_INET, SIM_IP, &dst.sin_addr);
    return s;
}

// ---------- Empfangen: stSeIsoStackState ----------
bool recv_state(int sock)
{
    uint8_t buffer[256]{};
    sockaddr_in src{};
    socklen_t slen = sizeof(src);

    // mit Timeout (select) arbeiten, damit wir Failsafe prüfen können
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sock, &fds);
    timeval tv{};
    tv.tv_sec = 0;
    tv.tv_usec = 200000; // 200 ms

    int ret = select(sock + 1, &fds, nullptr, nullptr, &tv);
    if (ret <= 0)
    {
        // kein Paket in diesem Zyklus
        return false;
    }

    ssize_t n = recvfrom(sock, buffer, sizeof(buffer), 0,
                         (sockaddr *)&src, &slen);
    if (n < (ssize_t)sizeof(stSeIsoStackState))
    {
        // Zu klein oder Fehler -> ignorieren
        return false;
    }

    // Achtung: Wir gehen davon aus, dass Sender und Empfänger gleiche Pack-Struktur nutzen
    stSeIsoStackState state{};
    std::memcpy(&state, buffer, sizeof(state));

    if (state.isoStackState.msgType != enIsoStackMsgType::SeCtrlState &&
        state.isoStackState.msgType != enIsoStackMsgType::CtrlState)
    {
        // falscher Typ -> ignorieren
        return false;
    }

    // mainContactor aus seHardwareCmd lesen
    uint8_t mc = state.seHardwareCmd.mainContactor;
    bool req = (mc != 0);
    g_mainReq = req;
    g_lastRx = std::chrono::steady_clock::now();

    // Nur ausgeben wenn sich etwas geändert hat
    if (req != g_lastMainReq)
    {
        std::cout << "[RX] mainContactor=" << (req ? "true" : "false") << "\n";
        std::cout << "> " << std::flush;
        g_lastMainReq = req;
    }
    return true;
}

// ---------- Senden: stSeIsoStackCmd (nur enable genutzt) ----------
void send_cmd(int sock, const sockaddr_in &dst)
{
    stSeIsoStackCmd cmd{};
    cmd.isoStackCmd.clear();
    cmd.seHardwareState.clear();

    cmd.isoStackCmd.msgVersion = 0;
    cmd.isoStackCmd.msgType = enIsoStackMsgType::SeCtrlCmd;
    cmd.isoStackCmd.enable = g_enable ? 1 : 0;
    cmd.seHardwareState.mainContactor = g_relayState ? 1 : 0;
    cmd.isoStackCmd.identification = 0; // erstmal unbenutzt
    cmd.isoStackCmd.currentDemand = 0;  // 0 A, später ggf. setzen

    // seHardwareState könnte man später nutzen, jetzt leer lassen
    // wichtig: Struct ist gepackt, direkt sendbar
    ssize_t n = sendto(sock,
                       &cmd,
                       sizeof(cmd),
                       0,
                       (const sockaddr *)&dst,
                       sizeof(dst));
    if (n < 0)
    {
        perror("sendto");
    }
    else
    {
        // std::cout << "[TX] enable=" << (g_enable ? "true" : "false") << "\n";
    }
}

// ---------- Hauptlogik ----------
int main()
{
    // Open log file
    g_logFile.open("/tmp/wallbox_main.log", std::ios::out | std::ios::app);
    if (!g_logFile.is_open())
    {
        std::cerr << "Warning: Could not open log file /tmp/wallbox_main.log" << std::endl;
    }

    std::signal(SIGINT, on_sigint);

    log_msg("INFO", "Wallbox Controller starting - Manual Mode");
    log_msg("INFO", std::string("Listening on UDP port ") + std::to_string(UDP_IN_PORT));
    log_msg("INFO", std::string("Sending to ") + SIM_IP + ":" + std::to_string(UDP_OUT_PORT));

    std::cout << "\n==========================================\n";
    std::cout << "  Wallbox Controller - Manual Mode\n";
    std::cout << "==========================================\n";
    std::cout << "Listening on UDP port " << UDP_IN_PORT << "\n";
    std::cout << "Sending to " << SIM_IP << ":" << UDP_OUT_PORT << "\n";
    std::cout << "Log file: /tmp/wallbox_main.log\n";

    init_gpio();
    g_lastRx = std::chrono::steady_clock::now();

    int sockIn = make_udp_in_sock();

    sockaddr_in dst{};
    int sockOut = make_udp_out_sock(dst);

    print_help();
    std::cout << "> " << std::flush;

    std::string input_buffer;

    while (g_run)
    {
        // 1) Status vom Simulator/Stack lesen (mainContactor)
        recv_state(sockIn);

        // 2) Failsafe prüfen
        auto now = std::chrono::steady_clock::now();
        bool timeout = (now - g_lastRx) > WATCHDOG_TMO;

        // 3) Entscheidung: Relais an/aus
        bool wantOn = false;
        if (!timeout && g_enable && g_mainReq)
        {
            wantOn = true;
        }

        // >>> HIER Debug-Meldung einbauen:
        if (g_mainReq && !g_enable)
        {
            // std::cout << "[INFO] Charge request from simulator, "
            "but Wallbox is DISABLED (enable=false)\n";
        }

        setElectricity(wantOn);

        // 4) eigenen Zustand (enable) zurückmelden
        send_cmd(sockOut, dst);

        // 5) Prüfe auf Benutzereingaben (non-blocking)
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
    setElectricity(false);
    close(sockIn);
    close(sockOut);

    log_msg("INFO", "Wallbox Control stopped");
    std::cout << "\nWallbox Control stopped.\n";

    // Close log file
    if (g_logFile.is_open())
    {
        g_logFile.close();
    }

    return 0;
}
