cmake_minimum_required(VERSION 3.10)

project(WallboxControl LANGUAGES CXX)

# Use C++17 (or higher if you want)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Executable (adjust src/main.cpp if your path is different)
add_executable(wallbox_control
    src/main.cpp
    src/IsoStackCtrlProtocol_impl.cpp
)

# Add include directory for IsoStackCtrlProtocol.h
# Adjust path if needed
target_include_directories(wallbox_control
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../LibPubWallbox
)

# On Unix-like systems, we might need extra system libs
if(UNIX)
    # Usually not needed for basic UDP, but if linker complains, uncomment:
    # target_link_libraries(wallbox_control PRIVATE pthread)
endif()
