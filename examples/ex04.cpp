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
const vchar_t dude{'@', YELLOW, BLACK};
// We're also going to render our destination as a pink heart. Aww.
const vchar_t destination_glyph{3, MAGENTA, BLACK};
// We now need to represent walls and floors, too
const vchar_t wall_tile{'#', WHITE, BLACK};
const vchar_t floor_tile{
    '.', GRAY, BLACK};  // Note that "floor" is taken as a name in C++!

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

        // Every tile other than 10,10 (starting) has a 16% chance of being
        // solid
        for(int y = 1; y < height - 2; ++y) {
            for(int x = 1; x < width - 2; ++x) {
                if((x != 10 && y != 10) && rng.dice_roll(1, 6) == 1)
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
        // std::cout << pos.x << "/" << pos.y << "\n";

        if(map.walkable(pos.x - 1, pos.y - 1))
            successors.push_back(location_t(pos.x - 1, pos.y - 1));
        if(map.walkable(pos.x, pos.y - 1))
            successors.push_back(location_t(pos.x, pos.y - 1));
        if(map.walkable(pos.x + 1, pos.y - 1))
            successors.push_back(location_t(pos.x + 1, pos.y - 1));

        if(map.walkable(pos.x - 1, pos.y))
            successors.push_back(location_t(pos.x - 1, pos.y));
        if(map.walkable(pos.x + 1, pos.y))
            successors.push_back(location_t(pos.x + 1, pos.y));

        if(map.walkable(pos.x - 1, pos.y + 1))
            successors.push_back(location_t(pos.x - 1, pos.y + 1));
        if(map.walkable(pos.x, pos.y + 1))
            successors.push_back(location_t(pos.x, pos.y + 1));
        if(map.walkable(pos.x + 1, pos.y + 1))
            successors.push_back(location_t(pos.x + 1, pos.y + 1));
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

// The A* library returns the A* path with a constrained template specialization
// to our location_t. Store the path here. Normally, you'd use "auto" for this
// type, it is a lot less typing!
std::shared_ptr<astar_path_t<location_t>> path;

// Instead of raw ints, we'll use the location structure to represent where our
// dude is. Using C++14 initialization, it's nice and clean.
auto dude_position = location_t{10, 10};

// We'll also use a location_t to represent the intended destination.
auto destination = location_t{10, 10};

// Lets go really fast!
constexpr auto moves_per_sec = 100.f;
constexpr auto tick_duration = 1.f / moves_per_sec;
auto tick_time               = 0.f;

// Tick is called every frame. The parameter specifies how many ms have elapsed
// since the last time it was called.
void tick(double duration_secs) {
    auto& [dude_x, dude_y] = dude_position;
    auto& [dest_x, dest_y] = destination;
    // Increase the tick time by the frame duration. If it has exceeded
    // the tick duration, then we move the @.
    tick_time += duration_secs;
    if(tick_time > tick_duration) {
        // Iterate over the whole map, rendering as appropriate
        for(int y = 0; y < MAP_HEIGHT; ++y) {
            for(int x = 0; x < MAP_WIDTH; ++x) {
                if(map.walkable(x, y)) {
                    vterm->set_char(vterm->at(x, y), floor_tile);
                } else {
                    vterm->set_char(vterm->at(x, y), wall_tile);
                }
            }
        }
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

            // Now determine how to get there
            if(path)
                path.reset();
            constexpr auto limit_steps = 500;
            path
                = path_find<navigator>(dude_position, destination, limit_steps);
            if(!path->success) {
                // std::cout << "RESET: THIS ISN'T MEANT TO HAPPEN!\n" <<;
                TraceLog(LOG_INFO,
                         "RESET, A* FAIL, SRC_POS: (%d, %d), DST_POS: (%d ,%d)",
                         dude_x, dude_y, dest_x, dest_y);
                vterm->set_char(dude_x, dude_y,
                                vchar_t{glyphs::SOLID, GREEN, BLANK});
                vterm->set_char(dest_x, dest_y,
                                vchar_t{glyphs::SOLID, RED, BLANK});
                destination = dude_position;
            }
        } else {
            vterm->dirty = true;
            // Follow the breadcrumbs!
            location_t next_step = path->steps.front();
            dude_x               = next_step.x;
            dude_y               = next_step.y;
            path->steps.pop_front();
        }

        // Important: we clear the tick count after the update.
        tick_time = 0.0;
    }

    // Render our planned path. We're using auto and a range-for to avoid typing
    // all the iterator stuff
    if(path) {
        // We're going to show off a bit and "lerp" the color along the path;
        // the red lightens as it approaches the destination. This is a preview
        // of some of the color functions.
        const auto n_steps = static_cast<float>(path->steps.size());
        auto i             = 0.f;
        for(auto step : path->steps) {
            const float lerp_amount = i / n_steps;
            vchar_t highlight{
                177,
                lerp(GREEN, colors::LIGHTEST_GREEN, lerp_amount),
                BLANK,
            };
            vterm->set_char(vterm->at(step.x, step.y), highlight);
            i += 1.f;
        }
    }

    // Render our destination
    vterm->set_char(vterm->at(dest_x, dest_y), destination_glyph);

    // Finally, we render the @ symbol. dude_x and dude_y are in terminal
    // coordinates.
    vterm->set_char(vterm->at(dude_x, dude_y), dude);
}

// Your main function
int main() {
    SetTargetFPS(200);
    // Initialize with defaults
    init(config_simple_px("../../resources"));

    // Enter the main loop. "tick" is the function we wrote above.
    run(tick);

    return 0;
}
