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
        return navigator_t::get_distance_estimate(pos, goal.pos);
    }

    bool IsGoal(search_node_t<location_t, navigator_t>& node_goal) {
        bool result = navigator_t::is_goal(pos, node_goal.pos);
        return result;
    }

    bool GetSuccessors(
        AStarSearch<search_node_t<location_t, navigator_t>>* a_star_search,
        search_node_t<location_t, navigator_t>* parent_node) {
        std::vector<location_t> successors;
        navigator_t::get_successors(pos, successors);
        for(const auto& loc : successors) {
            // skip the parent node, makes to set a backwards position as a
            // successor
            if(parent_node && (loc == parent_node->pos)) {
                continue;
            }
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
astar_path_t<location_t> path_find(location_t& start, location_t& end,
                                   size_t limit_steps = 100) {
    using user_node_t  = search_node_t<location_t, navigator_t>;
    auto a_start       = user_node_t(start);
    auto a_end         = user_node_t(end);
    auto a_star_search = AStarSearch<user_node_t>();

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

    auto result = astar_path_t<location_t>(false, end);
    if(search_state == AStarSearch<user_node_t>::SEARCH_STATE_SUCCEEDED) {
        for(auto* node = a_star_search.GetSolutionStart(); node;
            node       = a_star_search.GetSolutionNext()) {
            result.steps.push_back(node->pos);
        }
        a_star_search.FreeSolutionNodes();
        result.success = true;
    }
    a_star_search.EnsureMemoryFreed();
    return result;
}

}  // namespace radl
