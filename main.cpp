#include "raylib.h"
#include "color_t.hpp"
#include "texture.hpp"

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

    radl::texture tex("../resources/terminal16x16_gs_alpha.png");

    RenderTexture2D rtex;

    Rectangle texrect{16, 0, 16, 16};
    SetTargetFPS(144);
    // drawtexure
    // Main game loop
    while(!WindowShouldClose())  // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);
        auto& texture = *tex.tex;
        DrawTexture(texture, screenWidth / 2 - texture.width / 2,
                    screenHeight / 2 - texture.height / 2, WHITE);

        DrawTextureRec(texture, texrect, {0.F, 0.F}, PINK);
        DrawText("this IS a texture loaded from an image!", 300, 370, 10, GRAY);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    // UnloadTexture(texture);  // Texture unloading

    CloseWindow();  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
