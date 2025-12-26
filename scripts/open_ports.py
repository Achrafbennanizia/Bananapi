#!/usr/bin/env python3
"""
Wallbox Port Configuration Script
Opens all required network ports for the Wallbox system.

Required Ports:
- 8080  : HTTP REST API Server
- 50010 : UDP Listen Port (Simulator -> Pi)
- 50011 : UDP Send Port (Pi -> Simulator)
- 3000  : React Web App (optional, for development)
"""

import subprocess
import sys
import platform

# Define all required ports
PORTS = {
    8080: {"protocol": "tcp", "description": "HTTP REST API Server"},
    50010: {"protocol": "udp", "description": "UDP Listen Port (Simulator -> Pi)"},
    50011: {"protocol": "udp", "description": "UDP Send Port (Pi -> Simulator)"},
    3000: {"protocol": "tcp", "description": "React Web App (Development)"},
}


def run_command(cmd: list[str], check: bool = True) -> subprocess.CompletedProcess:
    """Run a shell command and return the result."""
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, check=check)
        return result
    except subprocess.CalledProcessError as e:
        print(f"Error running command: {' '.join(cmd)}")
        print(f"stderr: {e.stderr}")
        raise


def open_ports_macos():
    """Open ports on macOS using pfctl (packet filter)."""
    print("🍎 Detected macOS")
    print("ℹ️  macOS uses Application Firewall. Ports are typically open by default.")
    print("   If you need to allow specific apps through the firewall:")
    print("   1. Go to System Preferences > Security & Privacy > Firewall")
    print("   2. Click 'Firewall Options'")
    print("   3. Add your application to the allowed list")
    print()
    
    # Check if firewall is enabled
    result = run_command(["/usr/libexec/ApplicationFirewall/socketfilterfw", "--getglobalstate"], check=False)
    if "enabled" in result.stdout.lower():
        print("⚠️  Firewall is ENABLED")
        print("   You may need to allow incoming connections for your apps.")
    else:
        print("✅ Firewall is DISABLED - all ports should be accessible")
    
    print("\n📋 Required ports for Wallbox:")
    for port, info in PORTS.items():
        print(f"   - {port}/{info['protocol']}: {info['description']}")


def open_ports_linux():
    """Open ports on Linux using iptables or firewalld."""
    print("🐧 Detected Linux")
    
    # Check if firewalld is available
    firewalld_available = subprocess.run(
        ["which", "firewall-cmd"], capture_output=True
    ).returncode == 0
    
    # Check if ufw is available
    ufw_available = subprocess.run(
        ["which", "ufw"], capture_output=True
    ).returncode == 0
    
    # Check if iptables is available
    iptables_available = subprocess.run(
        ["which", "iptables"], capture_output=True
    ).returncode == 0
    
    if firewalld_available:
        print("📦 Using firewalld")
        open_ports_firewalld()
    elif ufw_available:
        print("📦 Using UFW (Uncomplicated Firewall)")
        open_ports_ufw()
    elif iptables_available:
        print("📦 Using iptables")
        open_ports_iptables()
    else:
        print("⚠️  No firewall management tool found!")
        print("   Please manually open the following ports:")
        for port, info in PORTS.items():
            print(f"   - {port}/{info['protocol']}: {info['description']}")


def open_ports_firewalld():
    """Open ports using firewalld."""
    print("\n🔓 Opening ports with firewalld...")
    
    for port, info in PORTS.items():
        protocol = info['protocol']
        cmd = ["sudo", "firewall-cmd", "--permanent", f"--add-port={port}/{protocol}"]
        print(f"   Opening {port}/{protocol} ({info['description']})...")
        try:
            run_command(cmd)
            print(f"   ✅ Port {port}/{protocol} opened")
        except Exception as e:
            print(f"   ❌ Failed to open port {port}/{protocol}: {e}")
    
    # Reload firewalld
    print("\n🔄 Reloading firewalld...")
    try:
        run_command(["sudo", "firewall-cmd", "--reload"])
        print("✅ Firewalld reloaded successfully")
    except Exception as e:
        print(f"❌ Failed to reload firewalld: {e}")
    
    # Show current rules
    print("\n📋 Current open ports:")
    result = run_command(["sudo", "firewall-cmd", "--list-ports"], check=False)
    print(f"   {result.stdout.strip()}")


def open_ports_ufw():
    """Open ports using UFW."""
    print("\n🔓 Opening ports with UFW...")
    
    for port, info in PORTS.items():
        protocol = info['protocol']
        cmd = ["sudo", "ufw", "allow", f"{port}/{protocol}"]
        print(f"   Opening {port}/{protocol} ({info['description']})...")
        try:
            run_command(cmd)
            print(f"   ✅ Port {port}/{protocol} opened")
        except Exception as e:
            print(f"   ❌ Failed to open port {port}/{protocol}: {e}")
    
    # Enable UFW if not enabled
    print("\n🔄 Ensuring UFW is enabled...")
    try:
        run_command(["sudo", "ufw", "--force", "enable"])
        print("✅ UFW enabled successfully")
    except Exception as e:
        print(f"⚠️  Could not enable UFW: {e}")
    
    # Show status
    print("\n📋 UFW Status:")
    result = run_command(["sudo", "ufw", "status", "verbose"], check=False)
    print(result.stdout)


def open_ports_iptables():
    """Open ports using iptables."""
    print("\n🔓 Opening ports with iptables...")
    
    for port, info in PORTS.items():
        protocol = info['protocol']
        cmd = [
            "sudo", "iptables", "-A", "INPUT",
            "-p", protocol,
            "--dport", str(port),
            "-j", "ACCEPT"
        ]
        print(f"   Opening {port}/{protocol} ({info['description']})...")
        try:
            run_command(cmd)
            print(f"   ✅ Port {port}/{protocol} opened")
        except Exception as e:
            print(f"   ❌ Failed to open port {port}/{protocol}: {e}")
    
    # Save iptables rules (distribution-dependent)
    print("\n💾 To persist rules across reboots, run:")
    print("   sudo iptables-save > /etc/iptables/rules.v4")
    print("   (or use your distribution's method)")
    
    # Show current rules
    print("\n📋 Current iptables INPUT rules:")
    result = run_command(["sudo", "iptables", "-L", "INPUT", "-n", "--line-numbers"], check=False)
    print(result.stdout)


def check_port_status():
    """Check if ports are currently in use."""
    print("\n🔍 Checking current port status...")
    
    for port, info in PORTS.items():
        # Check if port is listening
        if platform.system() == "Darwin":
            cmd = ["lsof", "-i", f":{port}"]
        else:
            cmd = ["ss", "-tuln", f"sport = :{port}"]
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        if result.stdout.strip():
            print(f"   ✅ Port {port} ({info['description']}): IN USE")
        else:
            print(f"   ⚪ Port {port} ({info['description']}): Available")


def main():
    """Main function to open all required ports."""
    print("=" * 60)
    print("🔌 Wallbox Port Configuration Script")
    print("=" * 60)
    
    system = platform.system()
    
    if system == "Darwin":
        open_ports_macos()
    elif system == "Linux":
        open_ports_linux()
    else:
        print(f"⚠️  Unsupported OS: {system}")
        print("   Please manually open the following ports:")
        for port, info in PORTS.items():
            print(f"   - {port}/{info['protocol']}: {info['description']}")
        sys.exit(1)
    
    check_port_status()
    
    print("\n" + "=" * 60)
    print("✅ Port configuration complete!")
    print("=" * 60)


if __name__ == "__main__":
    main()
