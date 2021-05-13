#include "font_manager.hpp"
#include "color_t.hpp"
#include "raylib.h"
#include "vchar.hpp"

namespace radl {

class virtual_terminal {
private:
    RenderTexture2D backing;  // backing texture
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
    std::vector<vchar_t> buffer;

public:
    int term_width;
    int term_height;
    bool visible = true;
    bool dirty   = true;  // Flag for requiring a re-draw

    virtual_terminal(const std::string fontt, const int x = 0, const int y = 0,
                     const bool background = true)
        : font_tag(fontt)
        , offset_x(x)
        , offset_y(y)
        , has_background(background) {
        backing = LoadRenderTexture(1920, 1080);
        font    = get_font(fontt);
    }

    ~virtual_terminal() {
        UnloadRenderTexture(backing);
    }

    /**
     * @brief  Helper function that returns the index of a character location in
     * the backing vector.
     *
     * @param x
     * @param y
     * @return size_t
     */
    inline size_t at(int x, int y) {
        return x + y * term_width;
    }


    /**
     * @brief clears the virtual terminal to blank (black) spaces
     */
    void clear();

    /**
     * @brief Set the char object
     *
     * @param index buffer index
     * @param vch
     */
    void set_char(int index, const vchar_t& vch) noexcept;

    inline void set_char(int x, int y, const vchar_t& vch) {
        set_char(at(x, y), vch);
    }

    /**
     * @brief Resize the terminal to match width x height pixels.
     */
    void resize_pixels(int width, int height) noexcept;

    /**
     * @brief Resize the terminal to match width x height virtual characters.
     */
    void resize_chars(int width, int height) noexcept;

    /**
     * @brief Use this to move the terminal in x/y screen pixels.
     */
    inline void set_offset(int x, int y) noexcept {
        offset_x = x;
        offset_y = y;
    };

    /**
     * @brief This gets the current font size in pixels.
     *
     * @return std::pair<int, int> -- width and height
     */
    inline std::pair<int, int> get_font_size() noexcept {
        return font->character_size;
    }

    /**
     * @brief Gets the font tag
     *
     * @return std::string
     */
    inline std::string get_font_tag() noexcept {
        return font->texture_tag;
    }

    void render();
};

}  // namespace radl
