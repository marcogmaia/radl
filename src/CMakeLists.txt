
include_directories(.)

add_library(
    radl
    "color_t.cpp"
    "virtual_terminal.cpp"
    "font_manager.cpp"
    "texture_resources.cpp"
)

target_link_libraries(radl raylib)
