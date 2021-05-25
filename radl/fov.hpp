#pragma once

#include "permissive-fov/permissive-fov.hpp"

namespace radl {

class IFov {
public:
    /**
     * @brief check if @p location is blocked
     *
     * @param x in the map
     * @param y in the map
     *
     * @return true if can't see through, false otherwise
     */
    virtual bool is_blocked(int x, int y) = 0;

    /**
     * @brief Visit a location in the map, we can perform some action, like: add
     * the visited location to a vector with visible locations
     *
     * @param x
     * @param y
     */
    virtual void visit(int x, int y) = 0;
};

}  // namespace radl
