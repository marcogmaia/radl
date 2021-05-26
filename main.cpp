#include <filesystem>

#include "color_t.hpp"
#include "radl.hpp"
#include "raylib.h"
#include "texture.hpp"
#include "texture_resources.hpp"
#include "virtual_terminal.hpp"

void test_radl(radl::virtual_terminal& vterm) {
    static double tick_time = 0.0;
    static radl::rng_t rng;
    static int dude_x = vterm.term_width / 2;
    static int dude_y = vterm.term_height / 2;
    static radl::vchar_t dude{'@', YELLOW, BLANK};
    tick_time += GetFrameTime();
    if(tick_time > 0.05) {
        // Using the RNG's handy dice_roll function, we roll 1d4. Each option
        // represents a possible move in this case. The function is just like
        // D&D's venerable XdY system - you can roll 10d12 with
        // dice_roll(10,12). You aren't limited to dice sizes that exist or make
        // sense.
        int direction = rng.dice_roll(1, 4);
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

void gui_layer_resize(radl::layer_t* layer, int new_width, int new_height) {
    layer->w = new_width;
    layer->h = new_height;
}

int main() {
    std::cout << "curr path: " << GetWorkingDirectory() << '\n';
    radl::config_simple cfg{"../resources"};
    radl::init(cfg);
    SetTargetFPS(200);

    while(!WindowShouldClose()) {
        // test_radl(gui.get_vterm(gui_handle));
        test_radl(*radl::vterm);

        BeginDrawing();
        ClearBackground(BLACK);
        // gui.render(radl::get_window());
        radl::vterm->render();
        radl::vterm->draw();
        DrawFPS(GetScreenWidth() - 100, 0);
        EndDrawing();
    }

    // No need to clear RADL initialized environment because of RAII idiom
    return 0;
}
