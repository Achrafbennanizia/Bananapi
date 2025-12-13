cmake_minimum_required(VERSION 3.10)
project(WallboxController VERSION 4.1 LANGUAGES CXX)

# C++14 Standard (for maximum compatibility)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find required packages
find_package(Threads REQUIRED)

# Check for libmicrohttpd
find_library(MICROHTTPD_LIBRARY microhttpd)
if(NOT MICROHTTPD_LIBRARY)
    message(FATAL_ERROR "libmicrohttpd not found. Install with: sudo apt install libmicrohttpd-dev")
endif()

# Check for libcurl
find_package(CURL REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CURL_INCLUDE_DIRS}
)

# Source files for wallbox controller
set(WALLBOX_SOURCES
    src/main_v3.cpp
    src/WallboxController.cpp
    src/ChargingStateMachine.cpp
    src/BananaPiGpioController.cpp
    src/StubGpioController.cpp
    src/UdpCommunicator.cpp
    src/HttpApiServer.cpp
    src/IsoStackCtrlProtocol_impl.cpp
    src/HardwareCpSignalReader.cpp
    src/SimulatorCpSignalReader.cpp
    src/CpSignalReaderFactory.cpp
)

# Source files for simulator
set(SIMULATOR_SOURCES
    src/simulator.cpp
    src/IsoStackCtrlProtocol_impl.cpp
)

# Wallbox Controller Executable
add_executable(wallbox_control_v3 ${WALLBOX_SOURCES})

target_link_libraries(wallbox_control_v3
    ${MICROHTTPD_LIBRARY}
    ${CURL_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${CMAKE_DL_LIBS}
)

# Simulator Executable
add_executable(simulator ${SIMULATOR_SOURCES})

target_link_libraries(simulator
    ${CURL_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

# Install targets
install(TARGETS wallbox_control_v3 simulator
    RUNTIME DESTINATION bin
)

install(FILES config/config.json
    DESTINATION etc/wallbox
)

# Copy config.json to build directory
configure_file(
    ${CMAKE_SOURCE_DIR}/config/config.json
    ${CMAKE_BINARY_DIR}/config.json
    COPYONLY
)

# Print configuration
message(STATUS "===================================")
message(STATUS "Wallbox Controller v4.1 Build Configuration")
message(STATUS "with CP Signal System Support")
message(STATUS "===================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Features: ISO 15118, CP Signal (HW+Simulator)")
message(STATUS "===================================")
