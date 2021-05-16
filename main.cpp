#include <fmt/format.h>
#include "raylib.h"
#include "color_t.hpp"
#include "texture.hpp"
#include "virtual_terminal.hpp"
#include "texture_resources.hpp"

#include "radl.hpp"

#include <filesystem>

void test_radl(radl::virtual_terminal& vterm) {
    static double tick_time = 0.0;
    static radl::rng_t rng;
    static int dude_x = vterm.term_width / 2;
    static int dude_y = vterm.term_height / 2;
    static radl::vchar_t dude{'@', YELLOW, BLANK};
    tick_time += GetFrameTime();
    if(tick_time > 0.05) {
        // Using the RNG's handy roll_dice function, we roll 1d4. Each option
        // represents a possible move in this case. The function is just like
        // D&D's venerable XdY system - you can roll 10d12 with
        // roll_dice(10,12). You aren't limited to dice sizes that exist or make
        // sense.
        int direction = rng.roll_dice(1, 4);
        switch(direction) {
        case 1: --dude_x; break;
        case 2: ++dude_x; break;
        case 3: --dude_y; break;
        case 4: ++dude_y; break;
        }

        // Important: we clear the tick count after the update.
        tick_time = 0.0;

        // Clear the console, which has the nice side effect of setting the
        // terminal to dirty.
        // vterm.clear(radl::vchar_t{'.', GRAY, BLACK});
        vterm.clear();
        // Clipping: keep the dude on the screen. Why are we doing this here,
        // and not after an update? For now, we aren't handling the concept of a
        // map that is larger than the screen - so if the window resizes, the @
        // gets clipped to a visible area.
        if(dude_x < 0)
            dude_x = 0;
        if(dude_x >= vterm.term_width)
            dude_x = vterm.term_width - 1;
        if(dude_y < 0)
            dude_y = 0;
        if(dude_y >= vterm.term_height)
            dude_y = vterm.term_height - 1;

        // Finally, we render the @ symbol. dude_x and dude_y are in terminal
        // coordinates.
        vterm.set_char(vterm.at(dude_x, dude_y), dude);
    }
}

int main() {
    std::cout << "curr path: " << GetWorkingDirectory() << '\n';
    constexpr int main_screen_width  = 800;
    constexpr int main_screen_height = 600;
    radl::config_advanced cfg{"../resources", main_screen_width,
                              main_screen_height};
    radl::init(cfg);
    SetTargetFPS(200);

    // TODO remove this from here, only testing to check if things are
    // working
    radl::vterm = std::make_unique<radl::virtual_terminal>("16x16");
    radl::vterm->resize_pixels(GetScreenWidth(), GetScreenHeight());
    radl::vterm->print(3, 3, "Marco A. G. Maia", BLUE, ORANGE);

    auto on_resize = []() {
        auto new_width               = GetScreenWidth();
        auto new_height              = GetScreenHeight();
        bool size_contraint_violated = false;
        if(new_width < main_screen_width) {
            size_contraint_violated = true;
            new_width               = main_screen_width;
        }
        if(new_height < main_screen_height) {
            size_contraint_violated = true;
            new_height              = main_screen_height;
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
        test_radl(*radl::vterm.get());

        BeginDrawing();
        ClearBackground(BLACK);
        radl::vterm->render(radl::get_window());
        DrawFPS(GetScreenWidth() - 100, 0);
        EndDrawing();
    }

    // No need to clear RADL initialized environment because of RAII idiom
    return 0;
}
