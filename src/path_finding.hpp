#include <memory>
#include <deque>
#include <utility>
#include <concepts>

#include "astar.hpp"
#include "geometry.hpp"

namespace radl {

// We want to force that the navigable class implemetns these methods
template <typename T>
concept CLocation = requires(const T loc) {
    { loc.get_x() }
    ->std::integral;
    { loc.get_y() }
    ->std::integral;
    { loc.get_xy() }
    ->std::convertible_to<std::tuple<int, int>>;
    std::equality_comparable<T>;
};

// Template class used to define what a navigation path looks like
template <CLocation location_t>
struct astar_path_t {
    // if true: the path was found, false otherwise
    bool success = false;
    location_t destination;
    // deque containing all the steps towards the destination after using the
    // path_find(start, end)
    std::deque<location_t> steps;
};


// The A* library also requires a helper class to understand your map format.
template <CLocation location_t, typename navigator_t>
class search_node_t final
    : public AStarState<search_node_t<location_t, navigator_t>> {
    using SearchNode_t = search_node_t<location_t, navigator_t>;

public:
    location_t pos;

    search_node_t() = default;
    explicit search_node_t(location_t loc)
        : pos(loc) {}

    float GoalDistanceEstimate(search_node_t<location_t, navigator_t>& goal) {
        auto& [xi, yi] = pos;
        auto& [xf, yf] = goal.pos;
        float d        = distance2d_squared(xi, yi, xf, yf);
        return d;
    }

    bool IsGoal(search_node_t<location_t, navigator_t>& node_goal) {
        bool result = navigator_t::is_goal(pos, node_goal.pos);
        return result;
    }

    bool GetSuccessors(
        AStarSearch<search_node_t<location_t, navigator_t>>* a_star_search,
        search_node_t<location_t, navigator_t>* parent_node) {
        // std::cout << "GetSuccessors called.\n";
        std::vector<location_t> successors;
        if(parent_node) {
            navigator_t::get_successors(parent_node->pos, successors);
        } else {
            navigator_t::get_successors(pos, successors);
        }
        for(location_t loc : successors) {
            search_node_t<location_t, navigator_t> tmp(loc);
            a_star_search->AddSuccessor(tmp);
        }
        return true;
    }

    float GetCost(search_node_t<location_t, navigator_t>& successor) {
        float result = navigator_t::get_cost(pos, successor.pos);
        return result;
    }

    bool IsSameState(search_node_t<location_t, navigator_t>& rhs) {
        bool result = navigator_t::is_same_state(pos, rhs.pos);
        return result;
    }
};

template <typename navigator_t, CLocation location_t>
std::shared_ptr<astar_path_t<location_t>>
path_find(location_t& start, location_t& end, size_t limit_steps = 100) {
    using user_node_t = search_node_t<location_t, navigator_t>;
    AStarSearch<search_node_t<location_t, navigator_t>> a_star_search;
    user_node_t a_start(start);
    user_node_t a_end(end);

    a_star_search.SetStartAndGoalStates(a_start, a_end);
    unsigned int search_state = 0;
    std::size_t search_steps  = 0;

    do {
        search_state = a_star_search.SearchStep();
        ++search_steps;
        if(search_steps > limit_steps) {
            a_star_search.CancelSearch();
        }
    } while(search_state == AStarSearch<user_node_t>::SEARCH_STATE_SEARCHING);

    if(search_state == AStarSearch<user_node_t>::SEARCH_STATE_SUCCEEDED) {
        auto result = std::make_shared<astar_path_t<location_t>>(false, end);
        auto* node  = a_star_search.GetSolutionStart();
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

    auto result = std::make_shared<astar_path_t<location_t>>(false, end);
    a_star_search.EnsureMemoryFreed();
    return result;
}

}  // namespace radl
