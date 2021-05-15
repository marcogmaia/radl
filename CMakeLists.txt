cmake_minimum_required(VERSION 3.15)

project(main)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(USER_CXX_FLAGS "-fdiagnostics-color=always")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USER_CXX_FLAGS}")

if(POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif()
find_package(OpenGL REQUIRED)
include_directories(lib/raylib/src)
include_directories(./src)
add_subdirectory(lib/raylib)
add_subdirectory(src)


add_executable(main main.cpp)

target_link_libraries(main radl)
