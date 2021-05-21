/*
 * Copyright (c) 2016-Present, Bracket Productions.
 * Licensed under the MIT license - see LICENSE file.
 *
 * Example 7: Introduction to complex GUIs. This example demonstrates how you
 * can create multiple layers, and use call-backs to resize them as the window
 * adjusts. It also displays a layer on top of another, with alpha transparency
 * (useful for effects).
 */

// You need to include the RADL header
#include "radl.hpp"
#include <sstream>
#include <iomanip>

// For convenience, import the whole radl namespace. You may not want to do this
// in larger projects, to avoid naming collisions.
using namespace radl;
using namespace radl::colors;

// For convenience, we'll define our GUI section handles here. These are just ID
// numbers.
enum {
    LAYER_TITLE = 0,
    LAYER_MAP,
    LAYER_DUDE,
    LOG_LAYER,
    LAYER_OVERLAY,
};
constexpr auto game_tick = 1.f / 20.f;
auto delta_time          = game_tick;

// Tick is called every frame. The parameter specifies how many ms have elapsed
// since the last time it was called.
void tick(double duration_secs) {
    delta_time += duration_secs;
    if(delta_time < game_tick) {
        return;
    }
    delta_time -= game_tick;


    // gui->clear();
    get_vterm(LAYER_TITLE).clear(vchar_t{' ', YELLOW, BLUE});
    get_vterm(LAYER_TITLE)
        .print_center(0, "    Big 32x32 Title    ", YELLOW, BLUE);
    // static float hue = 0.f;
    // auto huecolor    = ColorFromHSV(hue, 1.f, 1.f);
    // hue += 2.f;
    constexpr auto huecolor = BLANK;
    get_vterm(LAYER_MAP).clear(vchar_t{'.', GREY, huecolor});
    get_vterm(LAYER_MAP).box(GREY, BLACK, true);
    get_svterm(LAYER_DUDE).clear();
    get_svterm(LAYER_DUDE).set_char(10, 10, vchar_t{'@', YELLOW, BLACK});
    get_svterm(LAYER_DUDE).dirty = true;
    get_vterm(LOG_LAYER).clear(vchar_t{' ', WHITE, DARKEST_GREEN});
    get_vterm(LOG_LAYER).box(DARKEST_GREEN, BLACK);
    get_vterm(LOG_LAYER).print(1, 1, "Log Entry", LIGHT_GREEN, DARKEST_GREEN);
    get_vterm(LOG_LAYER).print(1, 2, "More text!", LIGHT_GREEN, DARKEST_GREEN);
    get_vterm(LOG_LAYER).print(1, 3, "Even more...", LIGHT_GREEN,
                               DARKEST_GREEN);
    get_vterm(LOG_LAYER).print(1, 4, "... goes here", LIGHT_GREEN,
                               DARKEST_GREEN);

    get_vterm(LAYER_OVERLAY).clear();
    constexpr auto bgcolor = BLANK;
    get_vterm(LAYER_OVERLAY)
        .set_char(11, 10,
                  vchar_t{17, LIGHT_GREEN, bgcolor});  // Draw a left arrow
    get_vterm(LAYER_OVERLAY)
        .print(12, 10, "Translucent Tool-tip", LIGHT_GREEN, bgcolor);

    std::stringstream ss;
    ss << std::setiosflags(std::ios::fixed) << std::setprecision(0)
       << (1.0 / duration_secs) << " FPS";
    get_vterm(LOG_LAYER).print(1, 6, ss.str(), WHITE, DARKEST_GREEN);
}

// This is called when the screen resizes, to allow the GUI to redefine itself.
void resize_title(layer_t* l, int w, int h) {
    // Simply set the width to the whole window width
    l->w = w;
    l->h = 32;  // Not really necessary - here for clarity
}

// This is called when the screen resizes, to allow the GUI to redefine itself.
void resize_main(layer_t* l, int w, int h) {
    // Simply set the width to the whole window width, and the whole window
    // minus 16 pixels (for the heading)
    l->w = w - 160;
    l->h = h - 32;
    if(l->w < 0)
        l->w = 160;  // If the width is too small with the log window, display
                     // anyway.
}

// This is called when the screen resizes, to allow the GUI to redefine itself.
void resize_log(layer_t* l, int w, int h) {
    // Simply set the width to the whole window width, and the whole window
    // minus 16 pixels (for the heading)
    l->w = w - 160;
    l->h = h - 32;

    // If the log window would take up the whole screen, hide it
    if(l->w < 0) {
        l->vterm->visible = false;
    } else {
        l->vterm->visible = true;
    }
    l->x = w - 160;
}

// Your main function
int main() {
    SetTargetFPS(200);
    // This time, we're using a full initialization: width, height, window
    // title, and "false" meaning we don't want an automatically generated root
    // console. This is necessary when you want to use the complex layout
    // functions.
    init(config_advanced("../../resources"));

    gui->add_layer(LAYER_TITLE, 0, 0, 1024, 32, "32x32", resize_title, true);
    gui->add_layer(LAYER_MAP, 0, 32, 1024 - 160, 768 - 32, "8x8", resize_main,
                   true);
    gui->add_sparse_layer(LAYER_DUDE, 0, 32, 1024 - 160, 768 - 32, "8x8",
                          resize_main, true);
    gui->add_layer(LOG_LAYER, 864, 32, 160, 768 - 32, "8x16", resize_log, true);
    gui->add_layer(LAYER_OVERLAY, 0, 32, 1024 - 160, 768 - 32, "8x8",
                   resize_main,
                   true);  // We re-use resize_main, we want it over the top
    get_vterm(LAYER_OVERLAY).set_alpha(196);  // Make the overlay translucent

    run(tick);

    return 0;
}
