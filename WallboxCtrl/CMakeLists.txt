cmake_minimum_required(VERSION 3.10)

project(WallboxControl LANGUAGES CXX)

# Use C++17 (or higher if you want)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../LibPubWallbox
)

# =============================================================================
# Legacy v1.0 Executable (original procedural code)
# =============================================================================
add_executable(wallbox_control
    src/main.cpp
    src/IsoStackCtrlProtocol_impl.cpp
)

target_link_libraries(wallbox_control PRIVATE pthread)

# =============================================================================
# New v2.0 Executable (SOLID architecture with design patterns)
# =============================================================================
add_executable(wallbox_control_v2
    src/main_v2.cpp
    src/WallboxController.cpp
    src/ChargingStateMachine.cpp
    src/StubGpioController.cpp
    src/UdpCommunicator.cpp
    src/IsoStackCtrlProtocol_impl.cpp
)

target_link_libraries(wallbox_control_v2 PRIVATE pthread)

# =============================================================================
# New v2.0 with REST API (For React app integration)
# =============================================================================
add_executable(wallbox_control_api
    src/main_v2_with_api.cpp
    src/WallboxController.cpp
    src/ChargingStateMachine.cpp
    src/StubGpioController.cpp
    src/BananaPiGpioController.cpp
    src/UdpCommunicator.cpp
    src/HttpApiServer.cpp
    src/IsoStackCtrlProtocol_impl.cpp
)

target_link_libraries(wallbox_control_api PRIVATE pthread)

# =============================================================================
# New v3.0 - Improved Architecture (Clean, modular design)
# =============================================================================
add_executable(wallbox_control_v3
    src/main_v3.cpp
    src/WallboxController.cpp
    src/ChargingStateMachine.cpp
    src/StubGpioController.cpp
    src/BananaPiGpioController.cpp
    src/UdpCommunicator.cpp
    src/HttpApiServer.cpp
    src/IsoStackCtrlProtocol_impl.cpp
)

target_link_libraries(wallbox_control_v3 PRIVATE pthread)

# =============================================================================
# Simulator (unchanged from v1.0)
# =============================================================================
add_executable(simulator
    src/simulator.cpp
    src/IsoStackCtrlProtocol_impl.cpp
)

target_link_libraries(simulator PRIVATE pthread)

# =============================================================================
# Installation
# =============================================================================
install(TARGETS wallbox_control wallbox_control_v2 wallbox_control_api wallbox_control_v3 simulator
    RUNTIME DESTINATION bin
)

# =============================================================================
# Optional: Unit Tests (requires Google Test)
# =============================================================================
option(BUILD_TESTS "Build unit tests" OFF)

if(BUILD_TESTS)
    find_package(GTest REQUIRED)
    enable_testing()
    
    add_executable(wallbox_tests
        tests/test_ChargingStateMachine.cpp
        tests/test_WallboxController.cpp
        src/ChargingStateMachine.cpp
        src/WallboxController.cpp
        src/StubGpioController.cpp
        src/UdpCommunicator.cpp
    )
    
    target_link_libraries(wallbox_tests
        PRIVATE
            GTest::GTest
            GTest::Main
            pthread
    )
    
    include(GoogleTest)
    gtest_discover_tests(wallbox_tests)
endif()

# Print build configuration
message(STATUS "==========================================")
message(STATUS "Wallbox Control Build Configuration")
message(STATUS "==========================================")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Build Tests: ${BUILD_TESTS}")
message(STATUS "==========================================")
message(STATUS "Targets:")
message(STATUS "  - wallbox_control (v1.0 legacy)")
message(STATUS "  - wallbox_control_v2 (v2.0 SOLID)")
message(STATUS "  - wallbox_control_api (v2.0 with REST API)")
message(STATUS "  - wallbox_control_v3 (v3.0 improved architecture)")
message(STATUS "  - simulator")
if(BUILD_TESTS)
    message(STATUS "  - wallbox_tests (unit tests)")
endif()
message(STATUS "==========================================")
