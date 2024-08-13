cmake_minimum_required(VERSION 3.15)

project(radl)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  set(USER_CXX_FLAGS "-fdiagnostics-color=always")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USER_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -g -Wall -DDEBUG")
endif()

add_subdirectory(external)

include_directories(radl)
add_subdirectory(radl)

set(COMPILE_MAIN OFF)
if(COMPILE_MAIN)
    add_executable(main main.cpp)
    target_link_libraries(main radl)
endif()

# compile examples
set(RADL_BUILD_EXAMPLES OFF CACHE BOOL "Build the examples")
if(${RADL_BUILD_EXAMPLES})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples")
    add_executable(ex01 examples/ex01.cpp)
    add_executable(ex02 examples/ex02.cpp)
    add_executable(ex03 examples/ex03.cpp)
    add_executable(ex04 examples/ex04.cpp)
    add_executable(ex05 examples/ex05.cpp)
    add_executable(ex06 examples/ex06.cpp)
    add_executable(ex07 examples/ex07.cpp)
    target_link_libraries(ex01 radl)
    target_link_libraries(ex02 radl)
    target_link_libraries(ex03 radl)
    target_link_libraries(ex04 radl)
    target_link_libraries(ex05 radl)
    target_link_libraries(ex06 radl)
    target_link_libraries(ex07 radl)
endif()
