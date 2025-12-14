# Wallbox Control System - Root CMake Configuration
# Version 4.1 - Industry Standard Structure

cmake_minimum_required(VERSION 3.10)

project(WallboxControlSystem VERSION 4.1.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Build mode from environment (production, development, debug)
if(NOT DEFINED BUILD_MODE)
    set(BUILD_MODE $ENV{BUILD_MODE})
endif()

if(NOT BUILD_MODE)
    set(BUILD_MODE "production")
endif()

message(STATUS "Build Mode: ${BUILD_MODE}")

# Compiler flags based on build mode
if(BUILD_MODE STREQUAL "debug")
    message(STATUS "Configuring DEBUG build with sanitizers")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fsanitize=undefined")
    add_definitions(-DDEBUG_MODE)
elseif(BUILD_MODE STREQUAL "development")
    message(STATUS "Configuring DEVELOPMENT build")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O1 -Wall -Wextra")
    add_definitions(-DDEVELOPMENT_MODE)
else()
    message(STATUS "Configuring PRODUCTION build")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
    add_definitions(-DPRODUCTION_MODE)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/wallbox
    ${CMAKE_SOURCE_DIR}/external/LibPubWallbox
)

# Platform detection
if(EXISTS "/sys/class/gpio")
    set(PLATFORM "BananaPi" CACHE STRING "Target platform")
    add_definitions(-DPLATFORM_BANANA_PI)
    message(STATUS "Detected BananaPi platform")
else()
    set(PLATFORM "Generic" CACHE STRING "Target platform")
    add_definitions(-DPLATFORM_GENERIC)
    message(STATUS "Using generic platform (stub GPIO)")
endif()

# Find dependencies
find_package(Threads REQUIRED)

# External dependencies - LibPubWallbox (ISO 15118)
file(GLOB LIBPUB_SOURCES 
    ${CMAKE_SOURCE_DIR}/external/LibPubWallbox/*.cpp
    ${CMAKE_SOURCE_DIR}/external/LibPubWallbox/GruppeC/*.cpp
)
# Exclude test_iso.cpp as it has its own main()
list(FILTER LIBPUB_SOURCES EXCLUDE REGEX ".*test_iso\\.cpp$")

# Core library sources
file(GLOB CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/core/*.cpp
)

file(GLOB GPIO_SOURCES
    ${CMAKE_SOURCE_DIR}/src/gpio/*.cpp
)

file(GLOB NETWORK_SOURCES
    ${CMAKE_SOURCE_DIR}/src/network/*.cpp
)

file(GLOB SIGNAL_SOURCES
    ${CMAKE_SOURCE_DIR}/src/signal/*.cpp
)

file(GLOB API_SOURCES
    ${CMAKE_SOURCE_DIR}/src/api/*.cpp
)

# Wallbox Control Core Library
add_library(wallbox_core STATIC
    ${CORE_SOURCES}
    ${GPIO_SOURCES}
    ${NETWORK_SOURCES}
    ${SIGNAL_SOURCES}
    ${LIBPUB_SOURCES}
)

target_include_directories(wallbox_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/wallbox
    ${CMAKE_SOURCE_DIR}/external/LibPubWallbox
)

target_link_libraries(wallbox_core
    Threads::Threads
)

# HTTP API Library
add_library(wallbox_api STATIC
    ${API_SOURCES}
)

target_link_libraries(wallbox_api
    wallbox_core
)

# Find system libcurl for simulator
find_library(CURL_LIB curl)
if(NOT CURL_LIB)
    message(STATUS "libcurl not found, simulator will be built without HTTP support")
    set(BUILD_SIMULATOR OFF)
else()
    message(STATUS "Found libcurl: ${CURL_LIB}")
    set(BUILD_SIMULATOR ON)
endif()

# Main executables

# Version 4 - Latest with full features (HTTP API, CP Signal, UDP)
add_executable(wallbox_control_v4
    ${CMAKE_SOURCE_DIR}/src/core/main_v4.cpp
)

target_link_libraries(wallbox_control_v4
    wallbox_core
    wallbox_api
)

# Version 3 - Previous simplified version (relay control only)
add_executable(wallbox_control_v3
    ${CMAKE_SOURCE_DIR}/src/core/main_v3.cpp
)

target_link_libraries(wallbox_control_v3
    wallbox_core
)

# Version 2 (SOLID architecture)
add_executable(wallbox_control_v2
    ${CMAKE_SOURCE_DIR}/src/core/main_v2.cpp
)

target_link_libraries(wallbox_control_v2
    wallbox_core
)

# Version 1
add_executable(wallbox_control_v1
    ${CMAKE_SOURCE_DIR}/src/core/main.cpp
)

target_link_libraries(wallbox_control_v1
    wallbox_core
)

# ISO 15118 Simulator
if(BUILD_SIMULATOR)
    add_executable(simulator
        ${CMAKE_SOURCE_DIR}/src/simulator/simulator.cpp
        ${LIBPUB_SOURCES}
    )

    target_include_directories(simulator PRIVATE
        ${CMAKE_SOURCE_DIR}/external/LibPubWallbox
    )

    target_link_libraries(simulator
        Threads::Threads
        ${CURL_LIB}
    )
else()
    message(WARNING "Simulator will not be built due to missing dependencies")
endif()

# Default target
if(BUILD_SIMULATOR)
    add_custom_target(default ALL
        DEPENDS wallbox_control_v4 simulator
    )
else()
    add_custom_target(default ALL
        DEPENDS wallbox_control_v4
    )
endif()

# Installation rules
if(BUILD_SIMULATOR)
    install(TARGETS wallbox_control_v4 simulator
        RUNTIME DESTINATION bin
    )
else()
    install(TARGETS wallbox_control_v4
        RUNTIME DESTINATION bin
    )
endif()

install(DIRECTORY ${CMAKE_SOURCE_DIR}/config/
    DESTINATION etc/wallbox
    FILES_MATCHING PATTERN "*.json"
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/scripts/
    DESTINATION share/wallbox/scripts
    FILES_MATCHING PATTERN "*.sh"
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                GROUP_EXECUTE GROUP_READ
                WORLD_EXECUTE WORLD_READ
)

# Testing (optional - requires Google Test)
option(BUILD_TESTS "Build unit tests" OFF)

if(BUILD_TESTS)
    enable_testing()
    find_package(GTest)
    
    if(GTest_FOUND)
        # Add integration tests
        file(GLOB TEST_SOURCES ${CMAKE_SOURCE_DIR}/tests/integration/*.cpp)
        foreach(TEST_SOURCE ${TEST_SOURCES})
            get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
            add_executable(${TEST_NAME} ${TEST_SOURCE})
            target_link_libraries(${TEST_NAME} wallbox_core GTest::GTest GTest::Main)
            add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
        endforeach()
    else()
        message(WARNING "Google Test not found, tests will not be built")
    endif()
endif()

# Print configuration summary
message(STATUS "==================================================")
message(STATUS "Wallbox Control System v${PROJECT_VERSION}")
message(STATUS "==================================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Build mode: ${BUILD_MODE}")
message(STATUS "Platform: ${PLATFORM}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "==================================================")
