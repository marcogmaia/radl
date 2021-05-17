#include <memory>
#include <deque>

#include "astar.hpp"

namespace radl {

// TODO make a constraint of navigator_t or an interface
template <typename location_t>
struct navigator_t {
    static int get_x(const location_t& pos) {
        return pos.first;
    }
    static int get_y(const location_t& pos) {
        return pos.second;
    }
    static location_t get_xy(int x, int y) {
        return location_t{x, y};
    }

    static bool is_walkable(const location_t& pos) {
        const auto& map = get_map();
        return map.rect.contains(pos) && map[pos].props.walkable
               && !is_occupied(pos);
    }

    // This lets you define a distance heuristic. Manhattan distance works
    // really well, but for now we'll just use a simple euclidian distance
    // squared. The geometry system defines one for us.
    static double get_distance_estimate(location_t& pos, location_t& goal) {
        auto& [xi, yi] = pos;
        auto& [xf, yf] = goal;
        float d        = distance2d_squared(xi, yi, xf, yf);
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
        for(int x = -1; x <= 1; ++x) {
            for(int y = -1; y <= 1; ++y) {
                if(x == 0 && y == 0) {
                    continue;
                }
                location_t offset{x, y};
                auto w_pos       = pos + offset;
                const auto& map  = get_map();
                auto& player_pos = reg.get<position_t>(player);
                if(w_pos == player_pos
                   || (map.rect.contains(w_pos) && map[w_pos].props.walkable
                       && !is_occupied(w_pos))) {
                    successors.push_back(w_pos);
                }
            }
        }
        return true;
    }

    // This function lets you set a cost on a tile transition. For now, we'll
    // always use a cost of 1.0.
    static double get_cost(location_t& position, location_t& successor) {
        return 1.0;
    }

    // This is a simple comparison to determine if two locations are the same.
    // It just passes through to the location_t's equality operator in this
    // instance (we didn't do that automatically) because there are times you
    // might want to behave differently.
    static bool is_same_state(const location_t& lhs, const location_t& rhs) {
        return lhs == rhs;
    }
};


template <typename location_t, typename navigator_t>
class map_search_node {
public:
    location_t pos;

    map_search_node() = default;
    explicit map_search_node(location_t loc)
        : pos(loc) {}

    float GoalDistanceEstimate(map_search_node<location_t, navigator_t>& goal) {
        float result = navigator_t::get_distance_estimate(pos, goal.pos);
        // std::cout << "GoalDistanceEstimate called (" << result << ").\n";
        return result;
    }

    bool IsGoal(map_search_node<location_t, navigator_t>& node_goal) {
        bool result = navigator_t::is_goal(pos, node_goal.pos);
        // std::cout << "IsGoal called (" << result << ").\n";
        return result;
    }

    bool GetSuccessors(
        AStarSearch<map_search_node<location_t, navigator_t>>* a_star_search,
        map_search_node<location_t, navigator_t>* parent_node) {
        // std::cout << "GetSuccessors called.\n";
        std::vector<location_t> successors;
        if(parent_node) {
            navigator_t::get_successors(parent_node->pos, successors);
        } else {
            map_search_node<location_t, navigator_t> tmp(pos);
            navigator_t::get_successors(pos, successors);
        }
        for(location_t loc : successors) {
            map_search_node<location_t, navigator_t> tmp(loc);
            // std::cout << " --> " << loc.first << "/" << loc.second << "\n";
            a_star_search->AddSuccessor(tmp);
        }
        return true;
    }

    float GetCost(map_search_node<location_t, navigator_t>& successor) {
        float result = navigator_t::get_cost(pos, successor.pos);
        // std::cout << "GetCost called (" << result << ").\n";
        return result;
    }

    bool IsSameState(map_search_node<location_t, navigator_t>& rhs) {
        bool result = navigator_t::is_same_state(pos, rhs.pos);
        // std::cout << "IsSameState called (" << result << ").\n";
        return result;
    }
};


// Template class used to define what a navigation path looks like
template <class location_t>
struct navigation_path {
    bool success = false;
    location_t destination;
    std::deque<location_t> steps;
};

template <typename location_t, typename navigator_t>
std::shared_ptr<navigation_path<location_t>>
find_path_2d(const location_t& start, const location_t& end,
             size_t limit_steps = 50) {
    {
        auto result = std::make_shared<navigation_path<location_t>>(true, end);
        auto x1     = navigator_t::get_x(start);
        auto y1     = navigator_t::get_y(start);
        auto x2     = navigator_t::get_x(end);
        auto y2     = navigator_t::get_y(end);
        auto dx     = x2 - x1;
        auto dy     = y2 - y1;
        auto ix     = (dx == 0) ? 0 : ((dx < 0) ? -1 : 1);
        auto iy     = (dy == 0) ? 0 : ((dy < 0) ? -1 : 1);
        x1 += ix;
        y1 += iy;
        line_func(x1, y1, x2, y2, [&result](int X, int Y) {
            location_t step = navigator_t::get_xy(X, Y);
            if(result->success && navigator_t::is_walkable(step)) {
                result->steps.push_back(step);
            } else {
                result->success = false;
                return;
            }
        });
        if(result->success) {
            return result;
        }
    }

    AStarSearch<map_search_node<location_t, navigator_t>> a_star_search;
    map_search_node<location_t, navigator_t> a_start(start);
    map_search_node<location_t, navigator_t> a_end(end);

    a_star_search.SetStartAndGoalStates(a_start, a_end);
    unsigned int search_state = 0;
    std::size_t search_steps  = 0;

    do {
        search_state = a_star_search.SearchStep();
        ++search_steps;
        if(search_steps > limit_steps) {
            a_star_search.CancelSearch();
        }
    } while(
        search_state
        == AStarSearch<
            map_search_node<navigator_t, location_t>>::SEARCH_STATE_SEARCHING);

    if(search_state
       == AStarSearch<
           map_search_node<navigator_t, location_t>>::SEARCH_STATE_SUCCEEDED) {
        auto result = std::make_shared<navigation_path<location_t>>(false, end);
        map_search_node<location_t, navigator_t>* node
            = a_star_search.GetSolutionStart();
        for(;;) {
            node = a_star_search.GetSolutionNext();
            if(!node) {
                break;
            }
            result->steps.push_back(node->pos);
        }
        a_star_search.FreeSolutionNodes();
        a_star_search.EnsureMemoryFreed();
        result->success = true;
        return result;
    }

    auto result = std::make_shared<navigation_path<location_t>>(false, end);
    a_star_search.EnsureMemoryFreed();
    return result;
}


/*
 * Implements a simple A-Star path, with no line-search optimization. This has
 * the benefit of avoiding requiring as much additional translation between the
 * template and your preferred map format, at the expense of being potentially
 * slower for some paths.
 */
template <class location_t, class navigator_t>
std::shared_ptr<navigation_path<location_t>> find_path(const location_t start,
                                                       const location_t end) {
    AStarSearch<map_search_node<location_t, navigator_t>> a_star_search;
    map_search_node<location_t, navigator_t> a_start(start);
    map_search_node<location_t, navigator_t> a_end(end);

    a_star_search.SetStartAndGoalStates(a_start, a_end);
    unsigned int search_state = 0;
    std::size_t search_steps  = 0;

    do {
        search_state = a_star_search.SearchStep();
        ++search_steps;
    } while(
        search_state
        == AStarSearch<
            map_search_node<navigator_t, location_t>>::SEARCH_STATE_SEARCHING);

    if(search_state
       == AStarSearch<
           map_search_node<navigator_t, location_t>>::SEARCH_STATE_SUCCEEDED) {
        auto result         = std::make_shared<navigation_path<location_t>>();
        result->destination = end;
        map_search_node<location_t, navigator_t>* node
            = a_star_search.GetSolutionStart();
        for(;;) {
            node = a_star_search.GetSolutionNext();
            if(!node) {
                break;
            }
            result->steps.push_back(node->pos);
        }
        a_star_search.FreeSolutionNodes();
        a_star_search.EnsureMemoryFreed();
        result->success = true;
        return result;
    }

    auto result = std::make_shared<navigation_path<location_t>>(false, end);

    a_star_search.EnsureMemoryFreed();
    return result;
}


}  // namespace radl
