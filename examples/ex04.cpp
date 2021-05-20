#include <iostream>
/*
 * Copyright (c) 2016-Present, Bracket Productions.
 * Licensed under the MIT license - see LICENSE file.
 *
 * Example 4: Now we implement a basic map, and use A* to find our way around
 * it. This example is a bit more in-depth, since it demonstrates the library's
 * ability to use templates to specialize itself around your map design - we
 * won't force a map type on you!
 */

// You need to include the RLTK header
#include "radl.hpp"
#include "path_finding.hpp"
#include "geometry.hpp"

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

// For now, we always want our "dude" to be a yellow @ - so he's constexpr
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
// We now need to represent walls and floors, too
const vchar_t wall_tile{
    glyphs::SOLID,
    WHITE,
    BLANK,
};
// Note that "floor" is taken as a name in C++!
const vchar_t floor_tile{
    glyphs::BLOCK1,
    GRAY,
    BLANK,
};

// Now we define a structure to represent a location. In this case, it's a
// simple x/y coordinate.
struct location_t {
    // Set uninitialized values to something invalid to help with debugging
    int x = -1;
    int y = -1;

    location_t() = default;

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

    // For convenience, we're overriding the quality operator. This gives a very
    // quick and natural looking way to say "are these locations the same?"
    bool operator==(const location_t& rhs) const {
        return (x == rhs.x && y == rhs.y);
    }
};


// Now we define our basic map. Why a struct? Because a struct is just a class
// with everything public in it!
struct map_t {
private:
    // The actual walkable storage vector
    std::vector<bool> m_walkable;

public:
    map_t(const int& w, const int& h)
        : width(w)
        , height(h) {
        // Resize the vector to hold the whole map; this way it won't reallocate
        m_walkable.resize(w * h);

        // Set the entire map to m_walkable
        std::fill(m_walkable.begin(), m_walkable.end(), true);

        // We want the perimeter to be solid
        for(int x = 0; x < width; ++x) {
            m_walkable[at(x, 0)]          = false;
            m_walkable[at(x, height - 1)] = false;
        }
        for(int y = 0; y < height; ++y) {
            m_walkable[at(0, y)]         = false;
            m_walkable[at(width - 1, y)] = false;
        }

        // Every tile other than 10,10 (starting) has a 25% chance of being
        // solid
        for(int y = 1; y < height - 2; ++y) {
            for(int x = 1; x < width - 2; ++x) {
                if((x != 10 && y != 10) && rng.dice_roll(1, 5) == 1)
                    m_walkable[at(x, y)] = false;
            }
        }
    }

    bool walkable(int x, int y) const {
        return m_walkable[at(x, y)];
    }

    bool walkable(int index) const {
        return m_walkable[index];
    }


    // Calculate the vector offset of a grid location
    inline int at(const int& x, const int& y) const {
        return x + (y * width);
    }

    // The width and height of the map
    const int width, height;
};

// We're using 1024x768, with 8 pixel wide chars. That gives a vterm grid of
// 128 x 96. We'll go with that for the map, even though in reality the screen
// might change. Worrying about that is for a future example!
constexpr int MAP_WIDTH  = 64;
constexpr int MAP_HEIGHT = 48;
map_t map(MAP_WIDTH, MAP_HEIGHT);

// The A* library also requires a helper class to understand your map format.
// This is it, all methods are static, this class should not be instantiated.
struct navigator {
    static int test(int x) {
        return x;
    }
    // This lets you define a distance heuristic. Manhattan distance works
    // really well, but for now we'll just use a simple euclidian distance
    // squared. The geometry system defines one for us.
    static float get_distance_estimate(location_t& pos, location_t& goal) {
        const auto& [px, py] = pos.get_xy();
        const auto& [gx, gy] = goal.get_xy();
        return distance2d_squared(px, py, gx, gy);
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
                if(map.walkable(sucessor.get_x(), sucessor.get_y())) {
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
};

// Instead of raw ints, we'll use the location structure to represent where our
// dude is. Using C++14 initialization, it's nice and clean.
auto dude_position = location_t{10, 10};

// We'll also use a location_t to represent the intended destination.
auto destination = location_t{10, 10};

// Lets go really fast!
constexpr auto moves_per_sec = 20.f;
constexpr auto tick_duration = 1.f / moves_per_sec;
auto tick_time               = tick_duration;


void draw_map() {
    auto& vterm1 = radl::gui->get_vterm(1);
    // draw_map
    for(int y = 0; y < MAP_HEIGHT; ++y) {
        for(int x = 0; x < MAP_WIDTH; ++x) {
            if(map.walkable(x, y)) {
                vterm1.set_char(x, y, floor_tile);
            } else {
                vterm1.set_char(x, y, wall_tile);
            }
        }
    }
}


// Tick is called every frame. The parameter specifies how many ms have elapsed
// since the last time it was called.
void tick(double duration_secs) {
    // The A* library returns the A* path with a constrained template
    // specialization to our location_t. Store the path here. Normally, you'd
    // use "auto" for this type, it is a lot less typing!
    static astar_path_t<location_t> path;
    auto& [dude_x, dude_y] = dude_position;
    auto& [dest_x, dest_y] = destination;


    auto& vterm2 = radl::gui->get_vterm(2);
    // Increase the tick time by the frame duration. If it has exceeded
    // the tick duration, then we move the @.
    tick_time += duration_secs;
    if(tick_time >= tick_duration) {
        // Important: subtract to even ou the fluctuations. Or maybe we could
        // clear the tick_time entirely
        tick_time -= tick_duration;
        vterm2.clear();
        // Iterate over the whole map, rendering as appropriate
        draw_map();
        // Are we there yet?
        if(dude_position == destination) {
            // We are there! We need to pick a new destination.
            dest_x = rng.dice_roll(1, MAP_WIDTH) - 1;
            dest_y = rng.dice_roll(1, MAP_HEIGHT) - 1;

            // Lets make sure that the destination is walkable
            while(map.walkable(dest_x, dest_y) == false) {
                dest_x = rng.dice_roll(1, MAP_WIDTH) - 1;
                dest_y = rng.dice_roll(1, MAP_HEIGHT) - 1;
            }

            constexpr auto limit_steps = 200;
            path
                = path_find<navigator>(dude_position, destination, limit_steps);
            if(!path.success) {
                // std::cout << "RESET: THIS ISN'T MEANT TO HAPPEN!\n" <<;
                TraceLog(LOG_INFO,
                         "RESET, A* FAIL, SRC_POS: (%d, %d), DST_POS: (%d ,%d)",
                         dude_x, dude_y, dest_x, dest_y);
                destination = dude_position;
            }
        } else if(!path.steps.empty()) {
            // Follow the breadcrumbs!
            location_t next_step = path.steps.front();
            dude_x               = next_step.x;
            dude_y               = next_step.y;
            path.steps.pop_front();
        }
    }

    // Render our planned path. We're using auto and a range-for to avoid typing
    // all the iterator stuff
    if(path.success) {
        // We're going to show off a bit and "lerp" the color along the path;
        // the red lightens as it approaches the destination. This is a preview
        // of some of the color functions.
        const auto n_steps = static_cast<float>(path.steps.size());
        auto i             = 0.f;
        for(auto step : path.steps) {
            const float lerp_amount = i / n_steps;
            vchar_t highlight{
                glyphs::BLOCK2,
                lerp(GREEN, colors::LIGHTEST_GREEN, lerp_amount),
                BLANK,
            };
            vterm2.set_char(step.x, step.y, highlight);
            i += 1.f;
        }
    }


    // Render our destination
    vterm2.set_char(dest_x, dest_y, destination_glyph);

    // Finally, we render the @ symbol. dude_x and dude_y are in terminal
    // coordinates.
    // vterm->set_char(vterm->at(dude_x, dude_y), dude);
    vterm2.set_char(dude_x, dude_y, dude);
}

void layer_resize_func(layer_t* l, int w, int h) {
    // do nothing
    return;
}

// Your main function
int main() {
    // set the desired FPS
    SetTargetFPS(200);
    // Initialize with advanced configs, so we can use gui to enable textures
    // over another
    init(config_advanced{"../../resources"});
    radl::gui->add_layer(1, 0, 0, map.width * 16, map.height * 16, "16x16",
                         layer_resize_func, false);
    radl::gui->add_layer(2, 0, 0, map.width * 16, map.height * 16, "16x16",
                         layer_resize_func, false);
    // Enter the main loop. "tick" is the function we wrote above.
    run(tick);

    return 0;
}
