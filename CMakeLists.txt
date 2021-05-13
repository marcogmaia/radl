cmake_minimum_required(VERSION 3.15)
project(main)

# find_package(raylib 3.5 REQUIRED) # Requires at least version 2.5.0
cmake_policy(SET CMP0072 NEW)
include_directories(lib/raylib/src)
include_directories(./src)
add_subdirectory(lib/raylib)
add_subdirectory(src)

set(CMAKE_C_STANDARD 20)

add_executable(main main.cpp)

target_link_libraries(main radl)
