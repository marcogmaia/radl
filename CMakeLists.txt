cmake_minimum_required(VERSION 3.15)

project(radl)

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

set(COMPILE_EXAMPLES OFF)
# compile examples
if(COMPILE_EXAMPLES)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples")
    add_executable(ex01 examples/ex01.cpp)
    add_executable(ex02 examples/ex02.cpp)
    add_executable(ex03 examples/ex03.cpp)
    target_link_libraries(ex01 radl)
    target_link_libraries(ex02 radl)
    target_link_libraries(ex03 radl)
endif()
