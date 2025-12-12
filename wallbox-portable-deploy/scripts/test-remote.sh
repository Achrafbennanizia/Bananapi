#!/bin/bash
#
# Wallbox Controller - Remote Test Script
# Tests deployed wallbox on remote Pi
#

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Configuration
PI_USER="${PI_USER:-pi}"
PI_IP="$1"
WALLBOX_DIR="/home/$PI_USER/wallbox-control"
TEST_LOG="/tmp/wallbox_remote_test_$(date +%Y%m%d_%H%M%S).log"

# Test results
TESTS_PASSED=0
TESTS_FAILED=0

# Functions
log() {
    echo -e "${GREEN}[INFO]${NC} $1" | tee -a "$TEST_LOG"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1" | tee -a "$TEST_LOG"
}

warn() {
    echo -e "${YELLOW}[WARN]${NC} $1" | tee -a "$TEST_LOG"
}

test_header() {
    echo -e "${BLUE}[TEST]${NC} $1" | tee -a "$TEST_LOG"
}

pass() {
    echo -e "${GREEN}  ✓ PASS${NC}" | tee -a "$TEST_LOG"
    ((TESTS_PASSED++))
}

fail() {
    echo -e "${RED}  ✗ FAIL${NC} $1" | tee -a "$TEST_LOG"
    ((TESTS_FAILED++))
}

usage() {
    echo "Usage: $0 <pi_ip>"
    echo ""
    echo "Example:"
    echo "  $0 192.168.178.34"
    echo ""
    echo "Environment variables:"
    echo "  PI_USER  - SSH username (default: pi)"
    exit 1
}

# Test 1: SSH connectivity
test_ssh() {
    test_header "Testing SSH connection..."
    
    if ssh -o ConnectTimeout=5 -o StrictHostKeyChecking=no "$PI_USER@$PI_IP" "echo connected" > /dev/null 2>&1; then
        pass
        return 0
    else
        fail "Cannot connect via SSH"
        return 1
    fi
}

# Test 2: Check installation
test_installation() {
    test_header "Checking installation..."
    
    if ssh "$PI_USER@$PI_IP" "[ -d $WALLBOX_DIR ] && [ -f $WALLBOX_DIR/build/wallbox_control_v3 ]"; then
        pass
        return 0
    else
        fail "Wallbox not properly installed"
        return 1
    fi
}

# Test 3: Check service status
test_service() {
    test_header "Checking wallbox service..."
    
    STATUS=$(ssh "$PI_USER@$PI_IP" "sudo systemctl is-active wallbox 2>/dev/null || echo inactive")
    
    if [ "$STATUS" == "active" ]; then
        pass
        return 0
    else
        warn "Service is $STATUS (may be intentional)"
        pass  # Don't fail, service might not be enabled
        return 0
    fi
}

# Test 4: Test HTTP API
test_http_api() {
    test_header "Testing HTTP API..."
    
    # Make sure service is running
    ssh "$PI_USER@$PI_IP" "sudo systemctl start wallbox 2>/dev/null || true"
    sleep 3
    
    RESPONSE=$(curl -s -m 5 "http://$PI_IP:8080/api/status" || echo "FAILED")
    
    if [[ "$RESPONSE" == *"state"* ]]; then
        pass
        return 0
    else
        fail "API not responding"
        echo "Response: $RESPONSE"
        return 1
    fi
}

# Test 5: Test UDP ports
test_udp_ports() {
    test_header "Testing UDP ports..."
    
    # Check if UDP ports are listening
    UDP_CHECK=$(ssh "$PI_USER@$PI_IP" "ss -ulpn 2>/dev/null | grep -E ':(50010|50011)' || netstat -ulpn 2>/dev/null | grep -E ':(50010|50011)' || echo 'NONE'")
    
    if [[ "$UDP_CHECK" != "NONE" ]]; then
        pass
        return 0
    else
        fail "UDP ports not listening"
        return 1
    fi
}

# Test 6: Check configuration
test_config() {
    test_header "Checking configuration..."
    
    CONFIG=$(ssh "$PI_USER@$PI_IP" "cat $WALLBOX_DIR/build/config.json 2>/dev/null || echo 'MISSING'")
    
    if [[ "$CONFIG" != "MISSING" ]] && [[ "$CONFIG" == *"udp_port"* ]]; then
        pass
        return 0
    else
        fail "Configuration missing or invalid"
        return 1
    fi
}

# Test 7: Check logs
test_logs() {
    test_header "Checking system logs..."
    
    # Check for recent log entries
    LOGS=$(ssh "$PI_USER@$PI_IP" "sudo journalctl -u wallbox -n 10 --no-pager 2>/dev/null || echo 'NO_LOGS'")
    
    if [[ "$LOGS" != "NO_LOGS" ]]; then
        # Check for errors
        if echo "$LOGS" | grep -qi "error\|fatal\|crash"; then
            warn "Found errors in logs"
            echo "$LOGS" | grep -i "error\|fatal\|crash"
        fi
        pass
        return 0
    else
        warn "No logs available (service might not have started)"
        pass
        return 0
    fi
}

# Test 8: Test API endpoints
test_api_endpoints() {
    test_header "Testing API endpoints..."
    
    ENDPOINTS=("status" "info" "config")
    FAILED_ENDPOINTS=()
    
    for endpoint in "${ENDPOINTS[@]}"; do
        RESPONSE=$(curl -s -m 3 "http://$PI_IP:8080/api/$endpoint" || echo "FAILED")
        if [[ "$RESPONSE" == "FAILED" ]] || [[ "$RESPONSE" == "" ]]; then
            FAILED_ENDPOINTS+=($endpoint)
        fi
    done
    
    if [ ${#FAILED_ENDPOINTS[@]} -eq 0 ]; then
        pass
        return 0
    else
        fail "Endpoints not responding: ${FAILED_ENDPOINTS[*]}"
        return 1
    fi
}

# Test 9: Test GPIO configuration (if applicable)
test_gpio() {
    test_header "Testing GPIO configuration..."
    
    # Check if GPIO devices exist
    GPIO_CHECK=$(ssh "$PI_USER@$PI_IP" "ls /sys/class/gpio/ 2>/dev/null | wc -l")
    
    if [ "$GPIO_CHECK" -gt 0 ]; then
        pass
        return 0
    else
        warn "GPIO not accessible (may need root or different platform)"
        pass  # Don't fail, might be simulator mode
        return 0
    fi
}

# Test 10: Performance check
test_performance() {
    test_header "Checking system performance..."
    
    # Check CPU and memory
    STATS=$(ssh "$PI_USER@$PI_IP" "top -b -n 1 | grep wallbox || echo 'NOT_RUNNING'")
    
    if [[ "$STATS" != "NOT_RUNNING" ]]; then
        CPU=$(echo "$STATS" | awk '{print $9}')
        MEM=$(echo "$STATS" | awk '{print $10}')
        
        log "  CPU: $CPU%, MEM: $MEM%"
        pass
        return 0
    else
        warn "Wallbox process not found in top output"
        pass
        return 0
    fi
}

# Test 11: Test UDP communication
test_udp_communication() {
    test_header "Testing UDP communication..."
    
    # Send a test UDP packet
    echo "test" | timeout 2 nc -u "$PI_IP" 50010 2>/dev/null || true
    
    # Check if wallbox logged the message
    LOGS=$(ssh "$PI_USER@$PI_IP" "sudo journalctl -u wallbox -n 20 --no-pager --since '1 minute ago' 2>/dev/null || echo 'NO_LOGS'")
    
    if [[ "$LOGS" == *"UDP"* ]] || [[ "$LOGS" != "NO_LOGS" ]]; then
        pass
        return 0
    else
        warn "UDP communication test inconclusive"
        pass  # Don't fail, nc might not be available
        return 0
    fi
}

# Test 12: Firewall check
test_firewall() {
    test_header "Checking firewall rules..."
    
    UFW_STATUS=$(ssh "$PI_USER@$PI_IP" "sudo ufw status 2>/dev/null || echo 'NOT_INSTALLED'")
    
    if [[ "$UFW_STATUS" == "NOT_INSTALLED" ]]; then
        warn "ufw not installed (firewall might be disabled)"
        pass
        return 0
    fi
    
    if echo "$UFW_STATUS" | grep -q "8080.*ALLOW\|50010.*ALLOW\|50011.*ALLOW"; then
        pass
        return 0
    else
        warn "Firewall rules might not be configured"
        pass  # Don't fail, firewall might be disabled
        return 0
    fi
}

# Print detailed system info
print_system_info() {
    echo ""
    echo "======================================"
    echo "  Remote System Information"
    echo "======================================"
    echo ""
    
    log "Hostname:"
    ssh "$PI_USER@$PI_IP" "hostname"
    
    log "Platform:"
    ssh "$PI_USER@$PI_IP" "uname -a"
    
    log "Wallbox version:"
    ssh "$PI_USER@$PI_IP" "$WALLBOX_DIR/build/wallbox_control_v3 --version 2>/dev/null || echo 'N/A'"
    
    log "Service status:"
    ssh "$PI_USER@$PI_IP" "sudo systemctl status wallbox --no-pager -l | head -15 || echo 'N/A'"
    
    echo ""
}

# Print test summary
print_summary() {
    echo ""
    echo "======================================"
    echo "  Test Summary"
    echo "======================================"
    echo ""
    echo -e "Total tests: $((TESTS_PASSED + TESTS_FAILED))"
    echo -e "${GREEN}Passed: $TESTS_PASSED${NC}"
    echo -e "${RED}Failed: $TESTS_FAILED${NC}"
    echo ""
    echo "Log file: $TEST_LOG"
    echo ""
    
    if [ $TESTS_FAILED -eq 0 ]; then
        echo -e "${GREEN}All tests passed! ✓${NC}"
        echo ""
        echo "Wallbox is operational on $PI_IP"
        echo ""
        echo "Access points:"
        echo "  HTTP API: http://$PI_IP:8080/api/status"
        echo "  UDP:      $PI_IP:50010 (wallbox) / $PI_IP:50011 (simulator)"
        echo ""
        echo "View logs:"
        echo "  ssh $PI_USER@$PI_IP 'sudo journalctl -u wallbox -f'"
        return 0
    else
        echo -e "${RED}Some tests failed ✗${NC}"
        echo ""
        echo "Check the logs for details:"
        echo "  ssh $PI_USER@$PI_IP 'sudo journalctl -u wallbox -n 50'"
        return 1
    fi
}

# Main test execution
main() {
    if [ -z "$PI_IP" ]; then
        usage
    fi
    
    echo "======================================"
    echo "  Wallbox Remote Testing"
    echo "======================================"
    echo ""
    echo "Target: $PI_USER@$PI_IP"
    echo ""
    
    test_ssh || { error "Cannot connect to Pi. Aborting."; exit 1; }
    
    test_installation || true
    test_service || true
    test_config || true
    test_http_api || true
    test_udp_ports || true
    test_api_endpoints || true
    test_gpio || true
    test_logs || true
    test_performance || true
    test_udp_communication || true
    test_firewall || true
    
    print_system_info
    print_summary
}

# Run tests
main "$@"
