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
    Texture2D tex = {0};
    std::string font_tag;
    int offset_x;
    int offset_y;
    int width;
    int height;
    color_t tint{255, 255, 255, 255};
    bitmap_font* font = nullptr;
    std::vector<svchar_t> buffer;
    std::unique_ptr<render_texture_t> backing;

public:
    int term_width;
    int term_height;
    bool visible = true;
    bool dirty   = true;  // Flag for requiring a re-draw

    virtual_terminal_sparse(const std::string fontt, const int x, const int y)
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

    inline void set_char(const svchar_t& target) {
        dirty = true;
        buffer.emplace_back(target);
    }

    inline void set_char(int x, int y, const vchar_t& vch) {
        dirty                       = true;
        const auto& [glyph, fg, bg] = vch;
        buffer.emplace_back(svchar_t{
            glyph,
            fg,
            static_cast<float>(x),
            static_cast<float>(y),
        });
    }

    void render();

    /**
     * @brief Draw the entire backing texture to the screen
     */
    inline void draw() {
        // NOTE: Render texture must be y-flipped due to default OpenGL
        // coordinates (left-bottom)
        DrawTextureRec(
            backing->render_texture.texture,
            // src rect
            Rectangle{
                0.f,
                0.f,
                static_cast<float>(backing->render_texture.texture.width),
                static_cast<float>(-backing->render_texture.texture.height),
            },
            // position
            Vector2{
                static_cast<float>(offset_x),
                static_cast<float>(offset_y),
            },
            tint);
    }
};

}  // namespace radl
