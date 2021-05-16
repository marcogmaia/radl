#include <fmt/format.h>
#include "raylib.h"
#include "color_t.hpp"
#include "texture.hpp"
#include "virtual_terminal.hpp"
#include "texture_resources.hpp"

#include "radl.hpp"

int main() {

    radl::config_advanced cfg{"../resources", 800, 600};
    radl::init(cfg);
    SetTargetFPS(200);

    // TODO remove this from here, only testing to check if things are working
    radl::vterm = std::make_unique<radl::virtual_terminal>("16x16");
    radl::vterm->resize_pixels(GetScreenWidth(), GetScreenHeight());
    radl::vterm->print(3, 3, "Marco A. G. Maia", BLUE, ORANGE);

    auto on_resize = []() {
        auto new_width               = GetScreenWidth();
        auto new_height              = GetScreenHeight();
        bool size_contraint_violated = false;
        if(new_width < 800) {
            size_contraint_violated = true;
            new_width               = 800;
        }
        if(new_height < 600) {
            size_contraint_violated = true;
            new_height              = 600;
        }
        if(size_contraint_violated) {
            SetWindowSize(new_width, new_height);
        }
    };

    while(!WindowShouldClose()) {
        if(IsWindowResized()) {
            on_resize();
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

    // No need to clear RADL initialized environment because of RAII idiom
    return 0;
}
