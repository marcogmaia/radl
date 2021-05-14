#include <algorithm>
#include <ranges>

#include "virtual_terminal.hpp"
#include "texture_resources.hpp"

namespace radl {


void virtual_terminal::set_char(const int index, const vchar_t& vch) noexcept {
    if(index >= buffer.size()) {
        return;
    }
    buffer[index] = vch;
}

void virtual_terminal::clear() {
    dirty = true;
    std::ranges::fill(buffer, vchar_t{32, {255, 255, 255}, {0, 0, 0}});
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

void virtual_terminal::render() {
    if(!visible) {
        return;
    }

    if(this->font == nullptr) {
        throw std::runtime_error("Font not loaded: " + font_tag);
    }

    if(dirty) {
        dirty = false;
        tex   = radl::get_texture(this->font->texture_tag);

        const auto font_width = static_cast<float>(font->character_size.first);
        const auto font_height
            = static_cast<float>(font->character_size.second);
        Rectangle render_rect{0, 0, font_width, font_height};

        BeginTextureMode(backing);
        ClearBackground(BLACK);
        float pos_x = 0;
        float pos_y = 0;
        for(auto& vch : buffer) {
            if(has_background) {
                auto& [r, g, b, a] = vch.background;
                DrawRectangleV(Vector2{pos_x * font_width, pos_y * font_height},
                               Vector2{font_width, font_height},
                               Color{r, g, b, a});
            }
            render_rect.x      = (vch.glyph % 16) * font_width;
            render_rect.y      = (vch.glyph / 16) * font_height;
            auto& [r, g, b, a] = vch.foreground;
            DrawTextureRec(tex, render_rect,
                           Vector2{pos_x * font_width, pos_y * font_height},
                           Color{r, g, b, a});
            ++pos_x;
            if(pos_x >= term_width) {
                pos_x = 0;
                ++pos_y;
            }
        }
        EndTextureMode();
    }
    draw();
}

void virtual_terminal::draw() {
    // NOTE: Render texture must be y-flipped due to default OpenGL coordinates
    // (left-bottom)
    DrawTextureRec(backing.texture,
                   Rectangle{0.F, 0.F,
                             static_cast<float>(backing.texture.width),
                             static_cast<float>(-backing.texture.height)},
                   Vector2{0.F, 0.F}, WHITE);
}

}  // namespace radl
