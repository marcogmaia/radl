/* RLTK (RogueLike Tool Kit) 1.00
 * Copyright (c) 2016-Present, Bracket Productions.
 * Licensed under the MIT license - see LICENSE file.
 *
 * Font manager
 */

#pragma once

#include <string>
// #include <utility>

namespace radl {

struct bitmap_font {
    const std::string texture_tag;
    const std::pair<int, int> character_size;
    
    bitmap_font(const std::string tag, const int tile_width,
                const int tile_height)
        : texture_tag(tag)
        , character_size({tile_width, tile_height}) {}
};

void register_font_directory(const std::string path);
bitmap_font* get_font(const std::string font_tag);
void register_font(const std::string font_tag, const std::string filename,
                   int tile_width = 8, int tile_height = 8);


}  // namespace radl