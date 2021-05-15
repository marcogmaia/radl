/* RLTK (RogueLike Tool Kit) 1.00
 * Copyright (c) 2016-Present, Bracket Productions.
 * Licensed under the MIT license - see LICENSE file.
 *
 * Provides support for complicated GUIs.
 */

#pragma once


#include <functional>
#include <unordered_map>
#include <memory>
#include <vector>

#include "layer_t.hpp"
#include "raylib.h"

namespace radl {

/*
 * The overall GUI - holds layers and handles render calls. Access via rltk::gui
 */
class gui_t {
private:
    int screen_width;
    int screen_height;
    int render_order = 0;

    std::unordered_map<int, layer_t> layers;

    inline void check_handle_uniqueness(const int handle) {
        auto finder = layers.find(handle);
        if(finder != layers.end())
            throw std::runtime_error("Adding a duplicate layer handle: "
                                     + std::to_string(handle));
    }

public:
    gui_t(const int w, const int h)
        : screen_width(w)
        , screen_height(h) {}
    void on_resize(const int w, const int h);
    void render(RenderTexture2D& render_texture);

    // Specialization for adding console layers
    void add_layer(const int handle, const int X, const int Y, const int W,
                   const int H, std::string font_name,
                   std::function<void(layer_t*, int, int)> resize_fun,
                   bool has_background = true, int order = -1);

    // Specialization for sparse layers
    void add_sparse_layer(const int handle, const int X, const int Y,
                          const int W, const int H, std::string font_name,
                          std::function<void(layer_t*, int, int)> resize_fun,
                          int order = -1);

    // Specialization for adding owner-draw layers
    void add_owner_layer(
        const int handle, const int X, const int Y, const int W, const int H,
        std::function<void(layer_t*, int, int)> resize_fun,
        std::function<void(layer_t*, RenderTexture2D&)> owner_draw_fun,
        int order = -1);
    void delete_layer(const int handle);
    layer_t* get_layer(const int handle);
};

}  // namespace radl
