
include_directories(.)

add_library(
    radl
    "color_t.cpp"
    "virtual_terminal.cpp"
)

target_link_libraries(radl raylib)
