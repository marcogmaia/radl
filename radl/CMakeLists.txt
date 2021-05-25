
include_directories(
    "."
)

add_library(
    radl
    "color_t.cpp"
    "font_manager.cpp"
    "gui.cpp"
    "input_handler.cpp"
    "layer_t.cpp"
    "radl.cpp"
    "texture_resources.cpp"
    "virtual_terminal_sparse.cpp"
    "virtual_terminal.cpp"
    "permissive-fov/permissive-fov.cpp"
)

target_link_libraries(radl raylib)
