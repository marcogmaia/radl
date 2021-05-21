/*
 * Copyright (c) 2016-Present, Bracket Productions.
 * Licensed under the MIT license - see LICENSE file.
 *
 * Example 1: A truly minimal hello world root vterm, demonstrating
 * how to get started with RADL.
 */

// You need to include the radl header
#include "radl.hpp"

// We're using a stringstream to build the hello world message.
#include <sstream>

// For convenience, import the whole radl namespace. You may not want to do this
// in larger projects, to avoid naming collisions.
using namespace radl;

// Tick is called every frame. The parameter specifies how many ms have elapsed
// since the last time it was called.
void tick(double duration_ms) {
    // In this case, we just want to print "Hello World" in white on black.
    if(vterm->dirty) {
        vterm->clear();
        vterm->print(1, 1, "Hello World", WHITE, BLACK);
    }
}

// Your main function
int main() {
    SetTargetFPS(200);
    // Initialize the library. Here, we are providing plenty of into so you can
    // see what is available. There's also the option to use config_simple_px to
    // configure by pixels rather than characters. The first parameter is the
    // path to the font files. The second and third parameters specify the
    // desired vterm size in screen characters (80x25). The fourth parameter is
    // the window title. The fifth parameter says that we'd like the default
    // vterm to use an 8x16 VGA font. Not so great for games, but easy to read!
    // The final parameter controls whether or not we want to go full screen.
    init(config_simple("../../resources/", 80, 25, "Hello World", "16x16",
                       false));

    // Enter the main loop. "tick" is the function we wrote above.
    run(tick);

    return 0;
}
