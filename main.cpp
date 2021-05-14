#include <fmt/format.h>
#include "raylib.h"
#include "color_t.hpp"
#include "texture.hpp"
#include "virtual_terminal.hpp"
#include "texture_resources.hpp"

int main() {
    const int screenWidth  = 800;
    const int screenHeight = 600;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "raylib [textures] - image loading");

    radl::register_font("16x16", "../resources/terminal16x16_gs_alpha.png", 16,
                        16);
    radl::virtual_terminal vterm("16x16");  // use font
    vterm.resize_pixels(GetScreenWidth(), GetScreenHeight());

    vterm.clear();
    vterm.set_char(1, 1,
                   radl::vchar_t{'@', radl::color_t{255, 255, 0},
                                 radl::color_t{255, 255, 0, 127}});
    vterm.set_char(2, 2,
                   radl::vchar_t{'!', radl::color_t{0, 255, 0},
                                 radl::color_t{255, 255, 0, 127}});

    SetTargetFPS(200);
    // Main game loop
    // Detect window close button or ESC key
    while(!WindowShouldClose()) {
        // Update
        if(IsWindowResized()) {
            vterm.resize_pixels(GetScreenWidth(), GetScreenHeight());
            vterm.clear();
            vterm.set_char(1, 1,
                           radl::vchar_t{'@', radl::color_t{255, 255, 0},
                                         radl::color_t{255, 255, 0, 127}});
            vterm.set_char(2, 2,
                           radl::vchar_t{'@', radl::color_t{0, 255, 0},
                                         radl::color_t{255, 0, 0, 127}});
        }

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);
        vterm.render();
        auto screen_width = GetScreenWidth();
        DrawFPS(screen_width - 100, 0);
        EndDrawing();
    }
    // De-Initialization
    CloseWindow();  // Close window and OpenGL context
    return 0;
}
