#include <algorithm>

#include <boost/range/combine.hpp>

#include "virtual_terminal.hpp"
#include "texture_resources.hpp"

#include "raymath.h"
#include "rlgl.h"
#include "external/glad.h"  // GLAD extensions loading library,

namespace radl {


void virtual_terminal::set_char(const int index, const vchar_t& vch) {
    if(index >= static_cast<int>(buffer.size())) {
        return;
    }
    buffer[index] = vch;
}

void virtual_terminal::resize_chars(const int width, const int height) {
    this->term_width              = width;
    this->term_height             = height;
    const auto& [fwidth, fheight] = font->character_size;
    // RAII automatically unloads the previous texture and loads a new one
    this->backing
        = std::make_unique<render_texture_t>(width * fwidth, height * fheight);
    buffer.resize(width * height);
    clear();
    buffer_prev.resize(width * height);
}

void virtual_terminal::resize_pixels(const int width_px, const int height_px) {
    int chars_width  = width_px / font->character_size.first;
    int chars_height = height_px / font->character_size.second;
    resize_chars(chars_width, chars_height);
}

void virtual_terminal::print(const int x, const int y, const std::string& str,
                             const color_t& fg, const color_t& bg) {
    int idx = at(x, y);
    for(const auto& ch : str) {
        set_char(idx, vchar_t{ch, fg, bg});
        ++idx;
    }
}

void virtual_terminal::print_center(const int y, const std::string& str,
                                    const color_t& fg, const color_t& bg) {
    auto init_x = term_width / 2 - str.size() / 2;
    print(init_x, y, str, fg, bg);
}

void virtual_terminal::box(const int x, const int y, const int w, const int h,
                           const color_t& fg, const color_t& bg,
                           bool double_lines) {
    // horizontal
    for(int i = 1; i < w; ++i) {
        if(!double_lines) {
            set_char(x + i, y, vchar_t{196, fg, bg});
            set_char(x + i, y + h, vchar_t{196, fg, bg});
        } else {
            set_char(x + i, y, vchar_t{205, fg, bg});
            set_char(x + i, y + h, vchar_t{205, fg, bg});
        }
    }
    // vertical
    for(int i = 1; i < h; ++i) {
        if(!double_lines) {
            set_char(x, y + i, vchar_t{179, fg, bg});
            set_char(x + w, y + i, vchar_t{179, fg, bg});
        } else {
            set_char(x, y + i, vchar_t{186, fg, bg});
            set_char(x + w, y + i, vchar_t{186, fg, bg});
        }
    }
    // corners
    if(!double_lines) {
        set_char(x, y, vchar_t{218, fg, bg});
        set_char(x + w, y, vchar_t{191, fg, bg});
        set_char(x, y + h, vchar_t{192, fg, bg});
        set_char(x + w, y + h, vchar_t{217, fg, bg});
    } else {
        set_char(x, y, vchar_t{201, fg, bg});
        set_char(x + w, y, vchar_t{187, fg, bg});
        set_char(x, y + h, vchar_t{200, fg, bg});
        set_char(x + w, y + h, vchar_t{188, fg, bg});
    }
}

static void set_rectangle_position_from_vchar(Rectangle& rect,
                                              const radl::vchar_t& vchar,
                                              const radl::bitmap_font& font) {
    const auto& [font_width, font_height] = font.character_size;
    // 16x16 because the CP437 grid is in this format
    rect.x = vchar.glyph % 16 * font_width;
    rect.y = vchar.glyph / 16 * font_height;
}


void virtual_terminal::render() {
    if(!visible) {
        return;
    }

    if(this->font == nullptr) {
        throw std::runtime_error("Font not loaded: " + font_tag);
    }

    if(dirty) {
        dirty = false;

        Vector2 font_size = {
            static_cast<float>(font->character_size.first),
            static_cast<float>(font->character_size.second),
        };
        Rectangle tex_src_rect{
            0,
            0,
            font_size.x,
            font_size.y,
        };

        BeginTextureMode(backing->render_texture);

        // clear everything that has changed
        // rlSetBlendFactors(GL_ONE, GL_ONE, GL_FUNC_SUBTRACT);
        BeginBlendMode(BLEND_SUBTRACT_COLORS);
        Vector2 pos{0.f, 0.f};
        assert(buffer_prev.size() == buffer.size());
        for(auto&& [prev_vch, vch] : boost::combine(buffer_prev, buffer)) {
            // if nothing changed
            const Vector2 pos_bg = Vector2Multiply(pos, font_size);
            if(vch == prev_vch) {
            } else {  // changed
                DrawRectangleV(pos_bg, font_size, BLANK);
            }
            // increment vchar position
            pos.x += 1.f;
            if(pos.x >= term_width) {
                pos.x = 0.f;
                pos.y += 1.f;
            }
        }
        EndBlendMode();

        tex = radl::get_texture(this->font->texture_tag);
        pos = Vector2{0.f, 0.f};
        for(auto&& [prev_vch, vch] : boost::combine(buffer_prev, buffer)) {
            // if nothing changed
            if(vch == prev_vch) {
            } else {  // changed
                prev_vch             = vch;
                const Vector2 pos_bg = Vector2Multiply(pos, font_size);
                if(has_background) {
                    DrawRectangleV(pos_bg, font_size, vch.background);
                }
                set_rectangle_position_from_vchar(tex_src_rect, vch, *font);
                DrawTextureRec(tex, tex_src_rect,
                               Vector2Multiply(pos, font_size), vch.foreground);
            }
            // increment vchar position
            pos.x += 1.f;
            if(pos.x >= term_width) {
                pos.x = 0.f;
                pos.y += 1.f;
            }
        }
    }


    EndTextureMode();
}

}  // namespace radl
