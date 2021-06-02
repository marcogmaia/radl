#include "virtual_terminal.hpp"
#include <algorithm>
#include <boost/range/combine.hpp>
#include <mutex>

#include "raymath.h"
#include "rlgl.h"
#include "texture_resources.hpp"

namespace radl {


void virtual_terminal::set_char(const int index, const vchar_t& vch) {
    auto lock       = std::lock_guard(m_mutex);
    m_buffer[index] = vch;
}

void virtual_terminal::fill(const vchar_t& vch) {
    auto lock = std::lock_guard(m_mutex);
    std::fill(m_buffer.begin(), m_buffer.end(), vch);
}

void virtual_terminal::resize_chars(const int width, const int height) {
    this->term_width              = width;
    this->term_height             = height;
    const auto& [fwidth, fheight] = m_font->character_size;
    // RAII automatically unloads the previous texture and loads a new one
    this->m_backing
        = std::make_unique<render_texture_t>(width * fwidth, height * fheight);
    m_buffer.resize(width * height);
    clear();
    m_buffer_prev.resize(width * height);
}

void virtual_terminal::resize_pixels(const int width_px, const int height_px) {
    int chars_width  = width_px / m_font->character_size.first;
    int chars_height = height_px / m_font->character_size.second;
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

    if(this->m_font == nullptr) {
        throw std::runtime_error("Font not loaded: " + m_font_tag);
    }

    if(dirty) {
        dirty = false;

        Vector2 font_size = {
            static_cast<float>(m_font->character_size.first),
            static_cast<float>(m_font->character_size.second),
        };
        Rectangle tex_src_rect{
            0,
            0,
            font_size.x,
            font_size.y,
        };

        BeginTextureMode(m_backing->render_texture);
        // clear everything that has changed
        BeginBlendMode(BLEND_SUBTRACT_COLORS);
        Vector2 pos{0.f, static_cast<float>(term_height - 1)};
        assert(m_buffer_prev.size() == m_buffer.size());
        for(auto&& [prev_vch, vch] : boost::combine(m_buffer_prev, m_buffer)) {
            const Vector2 pos_bg = Vector2Multiply(pos, font_size);
            if(vch != prev_vch) {  // vch changed
                if(m_has_background
                   && vch.background.a != 0) {  // has bg and alpha channel
                    rlSetBlendMode(BLEND_ALPHA);
                    DrawRectangleRec(
                        Rectangle{pos_bg.x, pos_bg.y, font_size.x, font_size.y},
                        vch.background);
                } else {
                    rlSetBlendMode(BLEND_SUBTRACT_COLORS);
                    DrawRectangleRec(
                        Rectangle{pos_bg.x, pos_bg.y, font_size.x, font_size.y},
                        BLANK);
                }
            }
            // increment vchar position
            pos.x += 1.f;
            if(pos.x >= term_width) {
                pos.x = 0.f;
                pos.y -= 1.f;
            }
        }
        EndBlendMode();

        m_tex = radl::get_texture(this->m_font->texture_tag);
        pos   = Vector2{0.f, static_cast<float>(term_height - 1)};
        for(auto&& [prev_vch, vch] : boost::combine(m_buffer_prev, m_buffer)) {
            // if vch has changed
            if(vch != prev_vch) {
                prev_vch             = vch;
                const Vector2 pos_bg = Vector2Multiply(pos, font_size);
                set_rectangle_position_from_vchar(tex_src_rect, vch, *m_font);
                DrawTextureRec(m_tex, tex_src_rect, pos_bg, vch.foreground);
            }
            // increment vchar position
            pos.x += 1.f;
            if(pos.x >= term_width) {
                pos.x = 0.f;
                pos.y -= 1.f;
            }
        }
        EndTextureMode();
    }
}

}  // namespace radl
