/*
 * Copyright (c) 2016-Present, Bracket Productions.
 * Licensed under the MIT license - see LICENSE file.
 *
 * Example 6: Extend example 5 to include visibility and map revealed status.
 */
#include <iostream>

// You need to include the RADL header
#include "radl.hpp"
#include "path_finding.hpp"

// We're using a vector to represent the map
#include <vector>

// We're also going to be using a shared_ptr to a map. Why shared? Because the
// library hands it off to you and it's up to you to use it; this provides some
// safety that it will be disposed when you are done with it.
#include <memory>

// For convenience, import the whole rltk namespace. You may not want to do this
// in larger projects, to avoid naming collisions.
using namespace radl;

// A default-defined random number generator. You can specify a seed to get
// the same results each time, but for now we're keeping it simple.
rng_t rng;

// For now, we always want our "dude" to be a yellow @ - so he's const
const vchar_t dude{
    '@',
    YELLOW,
    BLANK,
};
// We're also going to render our destination as a pink heart. Aww.
const vchar_t destination_glyph{
    glyphs::HEART,
    MAGENTA,
    BLANK,
};

namespace {

// create an enum to keep track of the gui's render order
enum gui_handle_t {
    G_MAP = 0,
    G_DUDE,
};

}  // namespace

// Now we define a structure to represent a location. In this case, it's a
// simple x/y coordinate.
struct location_t {
    int x = -1;  // I like to set uninitialized values to something invalid for
                 // help with debugging
    int y = -1;

    // For convenience, we're overriding the quality operator. This gives a very
    // quick and natural looking way to say "are these locations the same?"
    bool operator==(const location_t& rhs) const {
        return (x == rhs.x && y == rhs.y);
    }

    location_t() {}
    location_t(const int X, const int Y)
        : x(X)
        , y(Y) {}

    int get_x() const {
        return x;
    }

    int get_y() const {
        return y;
    }

    auto get_xy() const {
        return std::tuple{x, y};
    }
};

// Now we define our basic map. Why a struct? Because a struct is just a class
// with everything public in it!
struct map_t {
    map_t(const int& w, const int& h)
        : width(w)
        , height(h) {
        // Resize the vectors to hold the whole map; this way it won't
        // reallocate
        walkable.resize(w * h);
        revealed.resize(w * h);
        visible.resize(w * h);

        // Set the entire map to walkable, not visible and not revealed
        std::fill(walkable.begin(), walkable.end(), true);
        std::fill(revealed.begin(), revealed.end(), false);
        std::fill(visible.begin(), visible.end(), false);

        // We want the perimeter to be solid
        for(int x = 0; x < width; ++x) {
            walkable[at(x, 0)]          = false;
            walkable[at(x, height - 1)] = false;
        }
        for(int y = 0; y < height; ++y) {
            walkable[at(0, y)]         = false;
            walkable[at(width - 1, y)] = false;
        }

        // Every tile other than 10,10 (starting) has a 33% chance of being
        // solid. We've made it more likely to have obstacles, since we're no
        // longer relying on the RNG to find our way.
        for(int y = 1; y < height - 2; ++y) {
            for(int x = 1; x < width - 2; ++x) {
                if(rng.dice_roll(1, 3) == 1)
                    walkable[at(x, y)] = false;
            }
        }
        walkable[at(10, 10)] = true;
    }

    // Calculate the vector offset of a grid location
    inline int at(const int& x, const int& y) {
        return (y * width) + x;
    }

    // The width and height of the map
    const int width, height;

    // The actual walkable storage vector
    std::vector<bool> walkable;

    // Revealed: has a tile been shown yet?
    std::vector<bool> revealed;

    // Visible: is a tile currently visible?
    std::vector<bool> visible;
};

// We're using 1024x768, with 8 pixel wide chars. That gives a console grid of
// 128 x 96. We'll go with that for the map, even though in reality the screen
// might change. Worrying about that is for a future example!
constexpr int MAP_WIDTH  = 64;
constexpr int MAP_HEIGHT = 48;
map_t map(MAP_WIDTH, MAP_HEIGHT);

// Instead of raw ints, we'll use the location structure to represent where our
// dude is. Using C++14 initialization, it's nice and clean.
location_t dude_position{10, 10};

// We'll also use a location_t to represent the intended destination.
location_t destination{10, 10};

// The A* library also requires a helper class to understand your map format.
struct navigator {
    // This lets you define a distance heuristic. Manhattan distance works
    // really well, but for now we'll just use a simple euclidian distance
    // squared. The geometry system defines one for us.
    static float get_distance_estimate(location_t& pos, location_t& goal) {
        float d = distance2d_squared(pos.x, pos.y, goal.x, goal.y);
        return d;
    }

    // Heuristic to determine if we've reached our destination? In some cases,
    // you'd not want this to be a simple comparison with the goal - for
    // example, if you just want to be adjacent to (or even a preferred distance
    // from) the goal. In this case, we're trying to get to the goal rather than
    // near it.
    static bool is_goal(location_t& pos, location_t& goal) {
        return pos == goal;
    }

    // This is where we calculate where you can go from a given tile. In this
    // case, we check all 8 directions, and if the destination is walkable
    // return it as an option.
    static bool get_successors(location_t pos,
                               std::vector<location_t>& successors) {
        const auto& [px, py] = pos.get_xy();
        for(int off_x = -1; off_x <= 1; ++off_x) {
            for(int off_y = -1; off_y <= 1; ++off_y) {
                if(off_x == 0 && off_y == 0) {
                    continue;
                }
                auto sucessor = location_t{px + off_x, py + off_y};
                if(map.walkable[map.at(sucessor.x, sucessor.y)]) {
                    successors.push_back(sucessor);
                }
            }
        }
        return true;
    }

    // This function lets you set a cost on a tile transition. For now, we'll
    // always use a cost of 1.0.
    static float get_cost(location_t& position, location_t& successor) {
        return 1.0f;
    }

    // This is a simple comparison to determine if two locations are the same.
    // It just passes through to the location_t's equality operator in this
    // instance (we didn't do that automatically) because there are times you
    // might want to behave differently.
    static bool is_same_state(location_t& lhs, location_t& rhs) {
        return lhs == rhs;
    }

    // We're using the Bresneham's line optimization for pathing this time,
    // which requires a few extra static methods. These are designed to
    // translate between your map format and co-ordinates used by the library
    // (we don't want to force you to structure things a certain way).
    static int get_x(const location_t& loc) {
        return loc.x;
    }
    static int get_y(const location_t& loc) {
        return loc.y;
    }
    static location_t get_xy(const int& x, const int& y) {
        return location_t{x, y};
    }
    static bool is_walkable(const location_t& loc) {
        return map.walkable[map.at(loc.x, loc.y)];
    }
};

#include "fov.hpp"

// Helper function: calls the RADL visibility permissive-fov algorithm with
// an interface to assist in understanding our map format.
// Also we set as final so maybe the compiler can optimize away the virtual
// calls
class fov_assist final : public IFov {
public:
    bool is_blocked(int x, int y) override {
        return !map.walkable[map.at(x, y)];
    }

    void visit(int x, int y) override {
        map.revealed[map.at(x, y)] = true;
        map.visible[map.at(x, y)]  = true;
    }
} fov;  // fov object to pass to the algorithm fov_assist fov;

void draw_map() {
    auto& map_vterm = radl::gui->get_vterm(gui_handle_t::G_MAP);
    // Iterate over the whole map, rendering as appropriate
    static const auto bright_light = std::tuple{1.f, 1.f, 1.f};
    static const auto dim_light    = std::tuple{.5f, .5f, .5f};
    static const auto dark_light   = std::tuple{.25f, .25f, .25f};
    // floor colors:
    static const auto floor_color = color_t(36, 18, 4);
    static const auto lighten_up_floor
        = apply_colored_light(floor_color, bright_light);
    static const auto darkened_floor
        = apply_colored_light(floor_color, dim_light);
    static const auto not_seen_floor
        = apply_colored_light(floor_color, dark_light);
    // wall colors:
    static const auto wall_color = color_t(0, 31, 36);
    static const auto lighten_up_wall
        = apply_colored_light(wall_color, bright_light);
    static const auto darkened_wall
        = apply_colored_light(wall_color, dim_light);
    static const auto not_seen_wall
        = apply_colored_light(wall_color, dark_light);

    for(int x = 0; x < map.width; ++x) {
        for(int y = 0; y < map.height; ++y) {
            // Caching so we don't keep doing the calculation
            const int map_idx = map.at(x, y);
            if(map.walkable[map_idx]) {
                if(map.visible[map_idx]) {
                    // Visible tile: render full color
                    map_vterm.set_char(map_idx, vchar_t{
                                                    glyphs::BLOCK1,
                                                    lighten_up_floor,
                                                    BLANK,
                                                });
                } else if(map.revealed[map_idx]) {
                    // Revealed tile: render grey
                    map_vterm.set_char(map_idx, vchar_t{
                                                    glyphs::BLOCK1,
                                                    darkened_floor,
                                                    BLANK,
                                                });
                } else {
                    // We haven't seen it yet - darkest gray
                    map_vterm.set_char(map_idx, vchar_t{
                                                    glyphs::BLOCK1,
                                                    not_seen_floor,
                                                    BLANK,
                                                });
                }
            } else {
                if(map.visible[map_idx]) {
                    // Visible tile: render full color
                    map_vterm.set_char(map_idx, vchar_t{
                                                    glyphs::SOLID,
                                                    lighten_up_wall,
                                                    BLANK,
                                                });
                } else if(map.revealed[map_idx]) {
                    // Revealed tile: render grey
                    map_vterm.set_char(map_idx, vchar_t{
                                                    glyphs::SOLID,
                                                    darkened_wall,
                                                    BLANK,
                                                });
                } else {
                    // We haven't seen it yet - darkest gray
                    map_vterm.set_char(map_idx, vchar_t{
                                                    glyphs::SOLID,
                                                    not_seen_wall,
                                                    BLANK,
                                                });
                }
            }
        }
    }
}

// Lets go really fast!
constexpr double tick_duration = 0.05;
double tick_time               = tick_duration;

// Tick is called every frame. The parameter specifies how many ms have elapsed
// since the last time it was called.
void tick(double duration_secs) {
    // The A* library returns a navigation path with a template specialization
    // to our location_t. Store the path here. Normally, you'd use "auto" for
    // this type, it is a lot less typing!
    static astar_path_t<location_t> path;
    auto& dude_vterm = radl::gui->get_vterm(gui_handle_t::G_DUDE);

    // Increase the tick time by the frame duration. If it has exceeded
    // the tick duration, then we move the @.
    tick_time += duration_secs;
    if(tick_time >= tick_duration) {
        // Important: we clear the tick count after the update.
        tick_time = 0.0;
        gui->clear();

        draw_map();

        // Are we there yet?
        if(dude_position == destination) {
            // Now we poll the mouse to determine where we want to go
            // This requests the mouse position in PIXELS, and ties it into our
            // mouse_x/mouse_y variables.
            auto [mouse_x, mouse_y] = get_mouse_position();

            // Since we're using an 8x8, it's just a matter of dividing by 8 to
            // find the terminal-character coordinates. There will be a helper
            // function for this once we get into retained GUIs.
            const int terminal_x = mouse_x / 16;
            const int terminal_y = mouse_y / 16;

            // If the mouse is pointing at a walkable location, and the left
            // button is down - path to the mouse.
            const bool walkable = map.walkable[map.at(terminal_x, terminal_y)];
            if(walkable && get_mouse_button_state(MOUSE_BUTTON_LEFT)) {
                destination.x = terminal_x;
                destination.y = terminal_y;

                // Now determine how to get there
                path = path_find<navigator>(dude_position, destination);
                if(!path.success) {
                    destination = dude_position;
                    std::cout << "RESET: THIS ISN'T MEANT TO HAPPEN!\n";
                }
            } else if(walkable) {
                // If the mouse is not clicked, then path to the mouse cursor
                // for display only
                path = path_find<navigator, location_t>(
                    dude_position, location_t{terminal_x, terminal_y});
            }
        } else if(!path.steps.empty()) {
            // Follow the breadcrumbs!
            location_t next_step = path.steps.front();
            dude_position.x      = next_step.x;
            dude_position.y      = next_step.y;
            path.steps.pop_front();
            // Update the map visibility
            std::fill(map.visible.begin(), map.visible.end(), false);
            permissive::squareFov(dude_position.x, dude_position.y, 10, fov);
        }
    }

    // Render our planned path. We're using auto and a range-for to avoid typing
    // all the iterator stuff
    if(path.success) {
        // We're going to show off a bit and "lerp" the color along the path;
        // the red lightens as it approaches the destination. This is a preview
        // of some of the color functions.
        const float n_steps = static_cast<float>(path.steps.size());
        float i             = 0;
        for(auto step : path.steps) {
            const float lerp_amount = i / n_steps;
            vchar_t highlight;
            // If we're at our destination, we are showing possible paths -
            // highlight green; otherwise, highlight red to indicate that we are
            // en route.
            if(dude_position == destination) {
                highlight = {177,
                             lerp(colors::DARK_GREEN, colors::LIGHTEST_GREEN,
                                  lerp_amount),
                             BLACK};
            } else {
                highlight = {
                    177,
                    lerp(colors::DARK_RED, colors::LIGHTEST_RED, lerp_amount),
                    BLACK};
            }
            dude_vterm.set_char(step.x, step.y, highlight);
            ++i;
        }
    }

    // Render our destination
    dude_vterm.set_char(destination.x, destination.y, destination_glyph);

    // Finally, we render the @ symbol. dude_x and dude_y are in terminal
    // coordinates.
    dude_vterm.set_char(dude_position.x, dude_position.y, dude);
}

// Your main function
int main() {
    SetTargetFPS(200);
    // Initialize with advanced configs so we can make use of layer, and set the
    // proper order of rendering
    init(config_advanced("../../resources"));
    radl::gui->add_layer(gui_handle_t::G_MAP, 0, 0, map.width * 16,
                         map.height * 16, "16x16", nullptr, false,
                         gui_handle_t::G_MAP);
    radl::gui->add_layer(gui_handle_t::G_DUDE, 0, 0, map.width * 16,
                         map.height * 16, "16x16", nullptr, false,
                         gui_handle_t::G_DUDE);
    // We call the permissive-fov here, so the starting position is
    // revealed
    permissive::squareFov(dude_position.x, dude_position.y, 10, fov);
    // Enter the main loop. "tick" is the function we wrote above.
    run(tick);

    return 0;
}
