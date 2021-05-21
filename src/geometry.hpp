#pragma once

/**
 *
 * Copyright (c) 2016-Present, Bracket Productions.
 * Licensed under the MIT license - see LICENSE file.
 *
 * Random number generator class.
 */

#include <functional>
#include <utility>
#include <cmath>

namespace radl {

/*
 * From a given point x/y, project forward radius units (generally tiles) at an
 * angle of degrees_radians degrees (in radians).
 */
std::pair<int, int> project_angle(const int& x, const int& y,
                                  const double& radius,
                                  const double& degrees_radians) noexcept;

/*
 * Provides a correct 2D distance between two points.
 */
inline double distance2d(const int& x1, const int& y1, const int& x2,
                         const int& y2) noexcept {
    const double dx = (double)x1 - (double)x2;
    const double dy = (double)y1 - (double)y2;
    return std::sqrt((dx * dx) + (dy * dy));
}

/*
 * Provides a fast 2D distance between two points, omitting the square-root;
 * compare with other squared distances.
 */
inline double distance2d_squared(const int& x1, const int& y1, const int& x2,
                                 const int& y2) noexcept {
    auto dx = (double)x1 - (double)x2;
    auto dy = (double)y1 - (double)y2;
    return (dx * dx) + (dy * dy);
}

/*
 * Provides 2D Manhattan distance between two points.
 */
inline double distance2d_manhattan(const int& x1, const int& y1, const int& x2,
                                   const int& y2) noexcept {
    const double dx = (double)x1 - (double)x2;
    const double dy = (double)y1 - (double)y2;
    return std::abs(dx) + std::abs(dy);
}

/*
 * Provides a correct 3D distance between two points.
 */
inline double distance3d(const int& x1, const int& y1, const int& z1,
                         const int& x2, const int& y2, const int& z2) noexcept {
    const double dx = (double)x1 - (double)x2;
    const double dy = (double)y1 - (double)y2;
    const double dz = (double)z1 - (double)z2;
    return std::sqrt((dx * dx) + (dy * dy) + (dz * dz));
}

/*
 * Provides a fast 3D distance between two points, omitting the square-root;
 * compare with other squared distances.
 */
inline double distance3d_squared(const int& x1, const int& y1, const int& z1,
                                 const int& x2, const int& y2,
                                 const int& z2) noexcept {
    double dx = (double)x1 - (double)x2;
    double dy = (double)y1 - (double)y2;
    double dz = (double)z1 - (double)z2;
    return (dx * dx) + (dy * dy) + (dz * dz);
}

/*
 * Provides Manhattan distance between two 3D points.
 */
inline double distance3d_manhattan(const int& x1, const int& y1, const int& z1,
                                   const int& x2, const int& y2,
                                   const int& z2) noexcept {
    const double dx = (double)x1 - (double)x2;
    const double dy = (double)y1 - (double)y2;
    const double dz = (double)z1 - (double)z2;
    return std::abs(dx) + std::abs(dy) + std::abs(dz);
}

/*
 * Perform a function for each line element between x1/y1 and x2/y2. We used to
 * use Bresenham's line, but benchmarking showed a simple double-based setup to
 * be faster.
 */
inline void line_func(const int& x1, const int& y1, const int& x2,
                      const int& y2,
                      std::function<void(int, int)>&& func) noexcept {
    auto x               = static_cast<double>(x1) + 0.5F;
    auto y               = static_cast<double>(y1) + 0.5F;
    auto dest_x          = static_cast<const double>(x2);
    auto dest_y          = static_cast<const double>(y2);
    const double n_steps = distance2d(x1, y1, x2, y2);
    const auto steps     = static_cast<const int>(std::floor(n_steps) + 1);
    const double slope_x = (dest_x - x) / n_steps;
    const double slope_y = (dest_y - y) / n_steps;

    for(int i = 0; i < steps; ++i) {
        func(static_cast<int>(x), static_cast<int>(y));
        x += slope_x;
        y += slope_y;
    }
}

/*
 * Perform a function for each line element between x1/y1/z1 and x2/y2/z2. Uses
 * a 3D implementation of Bresenham's line algorithm.
 * https://gist.github.com/yamamushi/5823518
 */
template <typename F>
void line_func_3d(const int& x1, const int& y1, const int& z1, const int& x2,
                  const int& y2, const int& z2, F&& func) noexcept {
    double x = static_cast<double>(x1) + 0.5F;
    double y = static_cast<double>(y1) + 0.5F;
    double z = static_cast<double>(z1) + 0.5F;

    double length = distance3d(x1, y1, z1, x2, y2, z2);
    int steps     = static_cast<int>(std::floor(length));
    double x_step = (x - x2) / length;
    double y_step = (y - y2) / length;
    double z_step = (z - z2) / length;

    for(int i = 0; i < steps; ++i) {
        x += x_step;
        y += y_step;
        z += z_step;
        func(static_cast<int>(std::floor(x)), static_cast<int>(std::floor(y)),
             static_cast<int>(std::floor(z)));
    }
}

/*
 * Perform a function for each line element between x1/y1 and x2/y2. We used to
 * use Bresenham's algorithm, but benchmarking showed that a simple double based
 * vector was faster.
 */
template <typename F>
inline void line_func_cancellable(const int& x1, const int& y1, const int& x2,
                                  const int& y2, F&& func) noexcept {
    auto x      = static_cast<double>(x1) + 0.5;
    auto y      = static_cast<double>(y1) + 0.5;
    auto dest_x = static_cast<const double>(x2);
    auto dest_y = static_cast<const double>(y2);

    const double n_steps = distance2d(x1, y1, x2, y2);
    const int steps      = static_cast<const int>(std::floor(n_steps) + 1);
    const double slope_x = (dest_x - x) / n_steps;
    const double slope_y = (dest_y - y) / n_steps;

    for(int i = 0; i <= steps; ++i) {
        if(!func(static_cast<int>(x), static_cast<int>(y))) {
            return;
        }
        x += slope_x;
        y += slope_y;
    }
    func(x2, y2);
}

template <typename F>
inline void bresenham(double x1, double y1, double x2, double y2,
                      F&& func) noexcept {
    // Find Delta
    double dx = x2 - x1;
    double dy = y2 - y1;

    // Find Signs
    int sx = (dx >= 0) ? 1 : (-1);
    int sy = (dy >= 0) ? 1 : (-1);

    // Get Initial Points
    double x = x1;
    double y = y1;

    // Flag to check if swapping happens
    int is_swapped = 0;

    // Swap if needed
    if(std::abs(dy) > std::abs(dx)) {
        // swap dx and dy
        double tdx = dx;
        dx         = dy;
        dy         = tdx;

        is_swapped = 1;
    }

    // Decision parameter
    double p = 2 * (std::abs(dy)) - std::abs(dx);

    // Print Initial Point
    // putpixels(x, y);
    if(!func(x, y)) {
        return;
    }

    // Loop for dx times
    for(int i = 0; i <= std::abs(dx); i++) {
        // Depending on decision parameter
        if(p <= 0) {
            if(is_swapped == 0) {
                x = x + sx;
                // putpixels(x, y);
                if(!func(x, y)) {
                    // return;
                }
            } else {
                y = y + sy;
                // putpixels(x, y);
                if(!func(x, y)) {
                    // return;
                }
            }
            p = p + 2 * std::abs(dy);
        } else {
            x = x + sx;
            y = y + sy;
            // putpixels(x, y);
            if(!func(x, y)) {
                // return;
            }
            p = p + 2 * std::abs(dy) - 2 * std::abs(dx);
        }
    }
}


/*
 * Perform a function for each line element between x1/y1/z1 and x2/y2/z2. Uses
 * a 3D implementation of Bresenham's line algorithm.
 * https://gist.github.com/yamamushi/5823518
 */
template <typename F>
void line_func_3d_cancellable(const int& x1, const int& y1, const int& z1,
                              const int& x2, const int& y2, const int& z2,
                              F&& func) noexcept {
    auto x = static_cast<double>(x1) + 0.5;
    auto y = static_cast<double>(y1) + 0.5;
    auto z = static_cast<double>(z1) + 0.5;

    auto length = distance3d(x1, y1, z1, x2, y2, z2);
    auto steps  = static_cast<int>(std::floor(length));
    auto x_step = (x - x2) / length;
    auto y_step = (y - y2) / length;
    auto z_step = (z - z2) / length;

    for(int i = 0; i < steps; ++i) {
        x += x_step;
        y += y_step;
        z += z_step;
        const bool keep_going = func(static_cast<int>(std::floor(x)),
                                     static_cast<int>(std::floor(y)),
                                     static_cast<int>(std::floor(z)));
        if(!keep_going) {
            return;
        }
    }
}
}  // namespace radl
