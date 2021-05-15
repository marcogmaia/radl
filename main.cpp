#include <fmt/format.h>
#include "raylib.h"
#include "color_t.hpp"
#include "texture.hpp"
#include "virtual_terminal.hpp"
#include "texture_resources.hpp"

#include "radl.hpp"

int main() {
    // const int screenWidth  = 800;
    // const int screenHeight = 600;
    // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    // InitWindow(screenWidth, screenHeight, "raylib [textures] - image
    // loading");

    // radl::register_font("16x16", "../resources/terminal16x16_gs_alpha.png",
    // 16,
    //                     16);
    // radl::virtual_terminal vterm("16x16");  // use font
    // vterm.resize_pixels(GetScreenWidth(), GetScreenHeight());

    // vterm.clear();
    // vterm.set_char(1, 1,
    //                radl::vchar_t{'@', radl::color_t{255, 255, 0},
    //                              radl::color_t{255, 255, 0, 127}});
    // vterm.set_char(2, 2,
    //                radl::vchar_t{'!', radl::color_t{0, 255, 0},
    //                              radl::color_t{255, 255, 0, 127}});
    // vterm.print(3, 3, "Marco. A. G. Maia", BLUE, ORANGE);
    // RenderTexture2D render_backing = LoadRenderTexture(1920, 1080);

    // SetExitKey(KEY_NULL);
    // SetTargetFPS(200);
    // // Main game loop
    // // Detect window close button or ESC key
    // while(!WindowShouldClose()) {
    //     // Update
    //     if(IsWindowResized()) {
    //         vterm.resize_pixels(GetScreenWidth(), GetScreenHeight());
    //         vterm.clear();
    //         vterm.set_char(1, 1,
    //                        radl::vchar_t{'@', radl::color_t{255, 255, 0},
    //                                      radl::color_t{255, 255, 0, 127}});
    //         vterm.set_char(2, 2,
    //                        radl::vchar_t{'@', radl::color_t{0, 255, 0},
    //                                      radl::color_t{255, 0, 0, 127}});
    //         vterm.print(3, 3, "Marco. A. G. Maia", ORANGE, BLUE);
    //     }

    //     // Draw
    //     BeginDrawing();
    //     ClearBackground(BLACK);
    //     vterm.render(render_backing);
    //     DrawFPS(GetScreenWidth() - 100, 0);
    //     EndDrawing();
    // }
    // // De-Initialization
    // UnloadRenderTexture(render_backing);
    // CloseWindow();  // Close window and OpenGL context


    radl::config_advanced cfg{"../resources", 800, 600};
    radl::init(cfg);
    SetTargetFPS(200);

    // TODO remove this from here, only testing to check if things are working
    radl::vterm = std::make_unique<radl::virtual_terminal>("16x16");
    radl::vterm->resize_pixels(GetScreenWidth(), GetScreenHeight());
    radl::vterm->print(3, 3, "Marco A. G. Maia", BLUE, ORANGE);


    while(!WindowShouldClose()) {
        if(IsWindowResized()) {
            radl::vterm->dirty = true;
            radl::vterm->set_char(1, 1, radl::vchar_t{'@', BLUE, ORANGE});
            radl::vterm->print(3, 3, "Marco A. G. Maia", ORANGE, BLUE);
        }

        BeginDrawing();
        ClearBackground(BLACK);
        radl::vterm->render(radl::get_window());
        DrawFPS(GetScreenWidth() - 100, 0);
        EndDrawing();
    }

    // radl::vterm->set_char

    radl::terminate();
    return 0;
}
