/*
 * Provides a virtual console, in 'sparse' mode: it doesn't assume that you have
 * a regular grid, supports off-alignment rendering, rotation and similar. It is
 * optimized for use with a few characters, rather than a complete grid. A
 * common usage would be as a layer for PCs/NPCs, rendered over a regular
 * console grid.
 */

#pragma once

#include <vector>

#include "color_t.hpp"
#include "colors.hpp"
#include "font_manager.hpp"
#include "texture.hpp"
#include "vchar.hpp"

#include "raylib.h"

namespace radl {


class virtual_terminal_sparse {
private:
    Texture2D tex           = {0};
    std::string font_tag;
    int offset_x;
    int offset_y;
    uint8_t alpha = 255;
    color_t tint{255, 255, 255};
    bitmap_font* font = nullptr;
    std::vector<svchar_t> buffer;

public:
    int term_width;
    int term_height;
    bool visible = true;
    bool dirty   = true;  // Flag for requiring a re-draw

    virtual_terminal_sparse(const std::string fontt, const int x = 0,
                            const int y = 0)
        : font_tag(fontt)
        , offset_x(x)
        , offset_y(y) {
        font = get_font(fontt);
    }

    void resize_pixels(const int width, const int height);
    void resize_chars(const int width, const int height);
    inline void clear() {
        dirty = true;
        buffer.clear();
    }

    inline void add(const svchar_t& target) {
        dirty = true;
        buffer.emplace_back(target);
    }

    void render(RenderTexture2D& render_texture);
};

}  // namespace radl
