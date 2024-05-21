cmake_minimum_required(VERSION 3.10)

# Set the project name
project(Chip8Emu VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(Chip8Emu ${SOURCES})

include_directories(include/ external/raylib/include/)

if(WIN32)
target_link_libraries(Chip8Emu ${CMAKE_SOURCE_DIR}/external/raylib/lib/win64/raylib.lib)
target_link_libraries(Chip8Emu gdi32 winmm)
endif()


# Include directories
target_include_directories(Chip8Emu PUBLIC "${PROJECT_BINARY_DIR}" include)
