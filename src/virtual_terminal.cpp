#include <algorithm>
#include <ranges>

#include "virtual_terminal.hpp"
#include "texture_resources.hpp"

#include "raymath.h"
namespace radl {


void virtual_terminal::set_char(const int index, const vchar_t& vch) noexcept {
    if(index >= buffer.size()) {
        return;
    }
    buffer[index] = vch;
}

void virtual_terminal::clear() {
    dirty = true;
    std::ranges::fill(buffer, vchar_t{32, colors::NONE, colors::NONE});
}

void virtual_terminal::resize_chars(const int width,
                                    const int height) noexcept {
    this->dirty       = true;
    this->term_width  = width;
    this->term_height = height;
    buffer.resize(width * height);
}

void virtual_terminal::resize_pixels(const int width,
                                     const int height) noexcept {
    int chars_width  = width / font->character_size.first;
    int chars_height = height / font->character_size.second;
    resize_chars(chars_width, chars_height);
}

void virtual_terminal::print(int x, int y, const std::string& str,
                             const color_t& fg, const color_t& bg) noexcept {
    int idx = at(x, y);
    for(const auto& ch : str) {
        buffer[idx] = vchar_t{ch, fg, bg};
        ++idx;
    }
}

void virtual_terminal::box(const int x, const int y, const int w, const int h,
                           const color_t& fg, const color_t& bg,
                           bool double_lines) noexcept {
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

void virtual_terminal::render(RenderTexture2D& render_texture) {
    if(!visible) {
        return;
    }

    if(this->font == nullptr) {
        throw std::runtime_error("Font not loaded: " + font_tag);
    }

    if(dirty) {
        dirty = false;
        tex   = radl::get_texture(this->font->texture_tag);

        Vector2 font_size = {
            static_cast<float>(font->character_size.first),
            static_cast<float>(font->character_size.second),
        };
        Rectangle tex_src_rect{0, 0, font_size.x, font_size.y};

        BeginTextureMode(render_texture);
        ClearBackground(BLANK);
        Vector2 pos{0.F, 0.F};
        for(auto& vch : buffer) {
            if(has_background) {
                Vector2 pos_bg = Vector2Multiply(pos, font_size);
                DrawRectangleV(pos_bg, font_size, vch.background);
            }
            tex_src_rect.x = (vch.glyph % 16) * font_size.x;
            tex_src_rect.y = (vch.glyph / 16) * font_size.y;
            DrawTextureRec(tex, tex_src_rect, Vector2Multiply(pos, font_size),
                           vch.foreground);
            pos.x += 1.F;
            if(pos.x >= term_width) {
                pos.x = 0.F;
                pos.y += 1.F;
            }
        }
        EndTextureMode();
    }
}  // namespace radl

void virtual_terminal::draw(RenderTexture2D& render_texture) {
    // NOTE: Render texture must be y-flipped due to default OpenGL
    // coordinates
    // (left-bottom)
    DrawTextureRec(
        render_texture.texture,
        Rectangle{0.F, 0.F, static_cast<float>(render_texture.texture.width),
                  static_cast<float>(-render_texture.texture.height)},
        Vector2{0.F, 0.F}, WHITE);
}

}  // namespace radl
