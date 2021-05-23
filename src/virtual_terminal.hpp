#pragma once

#include <algorithm>
#include <ranges>
#include <execution>
#include <mutex>

#include "font_manager.hpp"
#include "color_t.hpp"
#include "raylib.h"
#include "vchar.hpp"

#include "colors.hpp"
#include "texture.hpp"

namespace radl {

class virtual_terminal {
private:
    Texture2D m_tex;
    std::string m_font_tag;
    int m_offset_x;
    int m_offset_y;
    color_t m_tint{255, 255, 255, 255};
    bool m_has_background;
    bitmap_font* m_font = nullptr;
    // we can have a list of position? that's what the buffer is for
    std::vector<vchar_t> m_buffer;
    std::vector<vchar_t> m_buffer_prev;
    std::unique_ptr<render_texture_t> m_backing;

    std::mutex m_mutex;

public:
    int term_width;
    int term_height;
    bool visible = true;
    bool dirty   = true;  // Flag for requiring a re-draw

    virtual_terminal(const std::string fontt, const int x = 0, const int y = 0,
                     const bool background = false)
        : m_font_tag(fontt)
        , m_offset_x(x)
        , m_offset_y(y)
        , m_has_background(background) {
        m_font = get_font(fontt);
    }

    ~virtual_terminal() = default;

    /**
     * @brief  Helper function that returns the index of a character location in
     * the buffer.
     *
     * @param x
     * @param y
     * @return size_t
     */
    inline size_t at(int x, int y) noexcept {
        return x + y * term_width;
    }

    /**
     * @brief Fills the terminal with @p vch
     *
     * @param vch
     */
    void fill(const vchar_t& vch) ;

    /**
     * @brief Fills the terminal with @p vch, and set the terminal to dirty
     *
     * @param vch vchar to fill the terminal with
     */
    inline void clear(const vchar_t& vch = vchar_t{0, BLANK, BLANK}) {
        dirty = true;
        fill(vch);
    }

    /**
     * @brief Set the char object
     *
     * @param index buffer index
     * @param vch
     */
    void set_char(int index, const vchar_t& vch);

    inline void set_char(int x, int y, const vchar_t& vch) {
        set_char(at(x, y), vch);
    }

    /**
     * @brief Resize the terminal to match width x height pixels.
     */
    void resize_pixels(int width_px, int height_px);

    /**
     * @brief Resize the terminal to match width x height virtual characters.
     */
    void resize_chars(int width, int height);

    /**
     * @brief Use this to move the terminal in x/y screen pixels.
     */
    inline void set_offset(int x, int y) noexcept {
        m_offset_x = x;
        m_offset_y = y;
    };

    /**
     * @brief This gets the current font size in pixels.
     *
     * @return std::pair<int, int> -- width and height
     */
    inline std::pair<int, int> get_font_size() noexcept {
        return m_font->character_size;
    }

    /**
     * @brief Gets the font tag
     *
     * @return std::string
     */
    inline std::string get_font_tag() noexcept {
        return m_font->texture_tag;
    }

    void print(int x, int y, const std::string& str,
               const color_t& fg = colors::White,
               const color_t& bg = colors::NONE);

    void print_center(int y, const std::string& str, const color_t& fg,
                      const color_t& bg);

    /**
     * @brief Draw a box at x/y of size w/h, in color fg/bg (or black/white if
     * not specified). If double_lines is true, it will use the two-line/thick
     * box characters.
     */
    void box(const int x, const int y, const int w, const int h,
             const color_t& fg, const color_t& bg, bool double_lines = false);

    /**
     * @brief Draw a box that encompasses the whole terminal boundary, in color
     * fg/bg. If double_lines is set to true, then it will use the
     * two-line/thick box characters.
     */
    inline void box(const color_t& fg = colors::White,
                    const color_t& bg = colors::NONE,
                    bool double_lines = false) {
        box(0, 0, term_width - 1, term_height - 1, fg, bg, double_lines);
    }

    /**
     * @brief render the virtual terminal to an existing texture
     *
     * @param render_texture
     */
    void render();

    /**
     * @brief Set the tint for the entire terminal
     *
     * @param color to tint with
     */
    inline void set_tint(const color_t& color) {
        m_tint = color;
    }

    /**
     * @brief Set the tint color alpha channel for the entire terminal
     *
     * @param alpha
     */
    inline void set_alpha(uint8_t alpha) {
        m_tint.a = alpha;
    }

    /**
     * @brief Draw the entire texture to the screen
     *
     * @param render_texture
     */
    inline void draw() {
        // NOTE: Render texture must be y-flipped due to default OpenGL
        // coordinates (left-bottom)
        DrawTextureRec(
            m_backing->render_texture.texture,
            // src rect
            Rectangle{
                0.f,
                0.f,
                static_cast<float>(m_backing->render_texture.texture.width),
                static_cast<float>(-m_backing->render_texture.texture.height),
            },
            // position
            Vector2{
                static_cast<float>(m_offset_x),
                static_cast<float>(m_offset_y),
            },
            m_tint);
    }
};

}  // namespace radl
