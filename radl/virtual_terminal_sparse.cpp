#include "virtual_terminal_sparse.hpp"

#include "raymath.h"
#include "texture_resources.hpp"

namespace radl {

void virtual_terminal_sparse::resize_pixels(const int width, const int height) {
    int w = width / font->character_size.first;
    int h = height / font->character_size.second;
    resize_chars(w, h);
}

void virtual_terminal_sparse::resize_chars(const int width, const int height) {
    dirty                         = true;
    term_width                    = width;
    term_height                   = height;
    const auto& [fwidth, fheight] = font->character_size;
    backing
        = std::make_unique<render_texture_t>(width * fwidth, height * fheight);
}

void virtual_terminal_sparse::render() {
    if(!visible)
        return;
    if(dirty) {
        dirty = false;
        static const Vector2 font_size{
            static_cast<float>(font->character_size.first),
            static_cast<float>(font->character_size.second),
        };
        static const auto tex = radl::get_texture(font->texture_tag);
        BeginTextureMode(backing->render_texture);
        ClearBackground(BLANK);
        Rectangle tex_rect_src{0, 0, font_size.x, font_size.y};
        for(auto& svch : buffer) {
            tex_rect_src.x = (svch.glyph % 16) * font_size.x;
            tex_rect_src.y = (svch.glyph / 16) * font_size.y;
            Vector2 pos{svch.x, svch.y};
            Vector2 render_pos = Vector2Multiply(pos, font_size);
            if(svch.has_background) {
                DrawRectangleV(render_pos, font_size, svch.background);
            }
            DrawTextureRec(tex, tex_rect_src, render_pos, svch.foreground);
        }
        EndTextureMode();
    }
}

}  // namespace radl
