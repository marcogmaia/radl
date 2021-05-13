cmake_minimum_required(VERSION 3.15)
project(radl)

# find_package(raylib 3.5 REQUIRED) # Requires at least version 2.5.0
add_subdirectory(lib/raylib)

set(CMAKE_C_STANDARD 11)

include_directories(./src)

add_executable(radl src/main.cpp)

target_link_libraries(radl raylib)
