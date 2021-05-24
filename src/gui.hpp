/* 
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
 * The overall GUI - holds layers and handles render calls. Access via radl::gui
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
    gui_t(int w, int h)
        : screen_width(w)
        , screen_height(h) {}

    void on_resize(int w, int h);

    void render();

    void draw(bool yflipped = false);

    /**
     * @brief draw to a RendeTexture
     * 
     * @param texture 
     */
    void draw(RenderTexture2D *texture);

    void clear();

    // Specialization for adding console layers
    void add_layer(int handle, int X, int Y, int W, int H,
                   std::string font_name,
                   std::function<void(layer_t*, int, int)> resize_fun = nullptr,
                   bool has_background = false, int order = -1);

    // Specialization for sparse layers
    void add_sparse_layer(int handle, int X, int Y, int W, int H,
                          std::string font_name,
                          std::function<void(layer_t*, int, int)> resize_fun,
                          int order = -1);

    // Specialization for adding owner-draw layers
    void add_owner_layer(
        int handle, int X, int Y, int W, int H,
        std::function<void(layer_t*, int, int)> resize_fun,
        std::function<void(layer_t*, RenderTexture2D&)> owner_draw_fun,
        int order = -1);
    void delete_layer(const int handle);

    layer_t* get_layer(const int handle);

    /**
     * @brief Gets the underlying virtual_terminal by reference
     *
     * @param handle
     * @return virtual_terminal&
     */
    inline virtual_terminal& get_vterm(int handle) {
        return *get_layer(handle)->vterm.get();
    }
};

}  // namespace radl
