#include "raylib.h"
#include "color_t.hpp"
#include "texture.hpp"
#include "virtual_terminal.hpp"
#include "texture_resources.hpp"

int main() {
    //--------------------------------------------------------------------------------------
    const int screenWidth  = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight,
               "raylib [textures] example - image loading");

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context
    // is required)

    // Image image = LoadImage(
    //     "../resources/terminal16x16_gs_alpha.png");  // Loaded in CPU memory
    //                                                  // (RAM)
    // Texture2D texture = LoadTextureFromImage(
    //     image);  // Image converted to texture, GPU memory (VRAM)

    // UnloadImage(image);  // Once image has been converted to texture and
    //                      // uploaded to VRAM, it can be unloaded from RAM
    //---------------------------------------------------------------------------------------

    radl::register_font("16x16", "../resources/terminal16x16_gs_alpha.png", 16,
                        16);
    radl::virtual_terminal vterm("16x16");
    vterm.resize_chars(32, 32);
    vterm.set_char(1, 1,
                   radl::vchar_t{
                       '@',
                       radl::color_t{255, 255, 0},
                       radl::color_t{255, 255, 0, 0},
                   });
    vterm.set_char(2, 2,
                   radl::vchar_t{
                       '@',
                       radl::color_t{0, 255, 0},
                       radl::color_t{255, 255, 0, 0},
                   });

    SetTargetFPS(144);

    //
    // radl::virtual_terminal vterm()


    // Main game loop
    while(!WindowShouldClose())  // Detect window close button or ESC key
    {
        // Update

        // Draw
        BeginDrawing();
        // ClearBackground(BLACK);
        vterm.render();
        // auto& texture = *tex.tex;
        // DrawTexture(texture, screenWidth / 2 - texture.width / 2,
        //             screenHeight / 2 - texture.height / 2, WHITE);

        // DrawTextureRec(texture, texrect, {0.F, 0.F}, PINK);
        // DrawText("this IS a texture loaded from an image!", 300, 370, 10,
        // GRAY);
        EndDrawing();
    }

    // De-Initialization
    CloseWindow();  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
