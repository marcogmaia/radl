#include "font_manager.hpp"
#include "color_t.hpp"
#include "raylib.h"
#include "vchar.hpp"

namespace radl {

class virtual_terminal {
private:
    RenderTexture2D backing;
    Texture2D tex;
    // used to get individual sprites from atlas
    // sf::VertexArray vertices;
    // we can have a list of position?
    std::string font_tag;
    int offset_x;
    int offset_y;
    uint8_t alpha = 255;
    color_t tint{255, 255, 255};
    bool has_background;
    bitmap_font* font = nullptr;
    std::vector<vchar> buffer;

public:
};

}  // namespace radl
