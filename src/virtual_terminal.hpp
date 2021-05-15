#include "font_manager.hpp"
#include "color_t.hpp"
#include "raylib.h"
#include "vchar.hpp"

#include "colors.hpp"
namespace radl {

class virtual_terminal {
private:
    // RenderTexture2D backing;  // backing texture
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
        // backing = LoadRenderTexture(1920, 1080);
        font = get_font(fontt);
    }

    ~virtual_terminal() {
        // UnloadRenderTexture(backing);
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

    void print(int x, int y, const std::string& str,
               const color_t& fg = colors::White,
               const color_t& bg = colors::NONE) noexcept;


    /**
     * @brief Draw a box at x/y of size w/h, in color fg/bg (or black/white if
     * not specified). If double_lines is true, it will use the two-line/thick
     * box characters.
     */
    void box(const int x, const int y, const int w, const int h,
             const color_t& fg, const color_t& bg,
             bool double_lines = false) noexcept;

    /**
     * @brief Draw a box that encompasses the whole terminal boundary, in color
     * fg/bg. If double_lines is set to true, then it will use the
     * two-line/thick box characters.
     */
    inline void box(const color_t& fg = colors::White,
                    const color_t& bg = colors::NONE,
                    bool double_lines = false) noexcept {
        box(0, 0, term_width - 1, term_height - 1, fg, bg, double_lines);
    }

    /**
     * @brief render the virtual terminal to a texture
     *
     * @param render_texture
     */
    void render(RenderTexture2D& render_texture);

    /**
     * @brief Draw the entire texture to the screen
     *
     * @param render_texture
     */
    static void draw(RenderTexture2D& render_texture);
};

}  // namespace radl
