#!/bin/bash
#
# Wallbox Controller - Local Test Script
# Tests the wallbox locally before deployment
#

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Configuration
BUILD_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../build" && pwd)"
TEST_LOG="/tmp/wallbox_test_$(date +%Y%m%d_%H%M%S).log"

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

# Test 1: Check build directory
test_build_dir() {
    test_header "Checking build directory..."
    
    if [ -d "$BUILD_DIR" ] && [ -f "$BUILD_DIR/wallbox_control_v3" ]; then
        pass
        return 0
    else
        fail "Build directory or binary not found"
        return 1
    fi
}

# Test 2: Check dependencies
test_dependencies() {
    test_header "Checking dependencies..."
    
    MISSING=()
    
    # Check libraries
    if ! ldconfig -p | grep -q libmicrohttpd; then
        MISSING+=("libmicrohttpd")
    fi
    
    if ! ldconfig -p | grep -q libcurl; then
        MISSING+=("libcurl")
    fi
    
    if [ ${#MISSING[@]} -eq 0 ]; then
        pass
        return 0
    else
        fail "Missing libraries: ${MISSING[*]}"
        return 1
    fi
}

# Test 3: Check configuration
test_configuration() {
    test_header "Checking configuration..."
    
    if [ -f "$BUILD_DIR/config.json" ]; then
        # Validate JSON
        if python3 -m json.tool "$BUILD_DIR/config.json" > /dev/null 2>&1; then
            pass
            return 0
        else
            fail "Invalid JSON in config.json"
            return 1
        fi
    else
        fail "config.json not found"
        return 1
    fi
}

# Test 4: Test binary execution
test_binary() {
    test_header "Testing binary execution..."
    
    cd "$BUILD_DIR"
    
    # Start wallbox in background
    timeout 3 ./wallbox_control_v3 > /tmp/wallbox_startup_test.log 2>&1 || true
    
    # Check if it started without immediate crash
    if grep -q "Wallbox Controller" /tmp/wallbox_startup_test.log; then
        pass
        return 0
    else
        fail "Binary failed to start properly"
        cat /tmp/wallbox_startup_test.log
        return 1
    fi
}

# Test 5: Check ports availability
test_ports() {
    test_header "Checking port availability..."
    
    PORTS=(8080 50010 50011)
    BLOCKED=()
    
    for port in "${PORTS[@]}"; do
        if lsof -Pi :$port -sTCP:LISTEN -t >/dev/null 2>&1; then
            BLOCKED+=($port)
        fi
    done
    
    if [ ${#BLOCKED[@]} -eq 0 ]; then
        pass
        return 0
    else
        warn "Ports in use: ${BLOCKED[*]} (may need to stop existing services)"
        pass  # Don't fail, just warn
        return 0
    fi
}

# Test 6: Start wallbox and test API
test_api_startup() {
    test_header "Starting wallbox and testing API..."
    
    cd "$BUILD_DIR"
    
    # Kill any existing instances
    pkill -9 wallbox_control 2>/dev/null || true
    sleep 1
    
    # Start wallbox
    ./wallbox_control_v3 </dev/null >/tmp/wallbox_api_test.log 2>&1 &
    WALLBOX_PID=$!
    
    # Wait for startup
    sleep 3
    
    # Test API
    RESPONSE=$(curl -s -m 5 http://localhost:8080/api/status || echo "FAILED")
    
    # Cleanup
    kill $WALLBOX_PID 2>/dev/null || true
    
    if [[ "$RESPONSE" == *"state"* ]]; then
        pass
        return 0
    else
        fail "API did not respond correctly"
        return 1
    fi
}

# Test 7: Test simulator
test_simulator() {
    test_header "Testing simulator..."
    
    if [ ! -f "$BUILD_DIR/simulator" ]; then
        warn "Simulator not built, skipping"
        return 0
    fi
    
    cd "$BUILD_DIR"
    
    # Test simulator startup
    echo "quit" | timeout 2 ./simulator > /tmp/simulator_test.log 2>&1 || true
    
    if grep -q "ISO 15118" /tmp/simulator_test.log; then
        pass
        return 0
    else
        fail "Simulator failed to start"
        return 1
    fi
}

# Test 8: Test UDP communication (basic)
test_udp_basic() {
    test_header "Testing UDP communication..."
    
    cd "$BUILD_DIR"
    
    # Start wallbox
    ./wallbox_control_v3 </dev/null >/tmp/wallbox_udp_test.log 2>&1 &
    WALLBOX_PID=$!
    sleep 2
    
    # Check if UDP port is listening
    if ss -ulpn 2>/dev/null | grep -q ":50010" || netstat -ulpn 2>/dev/null | grep -q ":50010"; then
        pass
        kill $WALLBOX_PID 2>/dev/null || true
        return 0
    else
        fail "UDP port not listening"
        kill $WALLBOX_PID 2>/dev/null || true
        return 1
    fi
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
        echo "System is ready for deployment."
        echo "Run: ./scripts/deploy.sh <pi_ip>"
        return 0
    else
        echo -e "${RED}Some tests failed ✗${NC}"
        echo ""
        echo "Fix the issues above before deploying."
        return 1
    fi
}

# Main test execution
main() {
    echo "======================================"
    echo "  Wallbox Local Testing"
    echo "======================================"
    echo ""
    
    test_build_dir || true
    test_dependencies || true
    test_configuration || true
    test_binary || true
    test_ports || true
    test_api_startup || true
    test_simulator || true
    test_udp_basic || true
    
    print_summary
}

# Run tests
main "$@"
