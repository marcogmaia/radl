#pragma once

#include <tuple>
#include <cereal/cereal.hpp>

#include "raylib.h"

namespace radl {

/* Converts HSV into an RGB tuple */
extern std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>
color_from_hsv(const float hue, const float saturation, const float value);

struct color_t {
    /* RGB storage */
    uint8_t r = 0x0;
    uint8_t g = 0x0;
    uint8_t b = 0x0;
    uint8_t a = 0xFF;

    /* Default empty constructor */
    color_t() {}

    color_t(Color rlcolor)
        : r{rlcolor.r}
        , g{rlcolor.g}
        , b{rlcolor.b}
        , a{rlcolor.a} {}

    // struct Color;
    inline operator Color() const {
        return Color{r, g, b, a};
    }

    /* Convenience constructor from red/green/blue; accepts ints and casts them
     */
    color_t(const int R, const int G, const int B, const int A = 0xFF)
        : r(static_cast<uint8_t>(R))
        , g(static_cast<uint8_t>(G))
        , b(static_cast<uint8_t>(B))
        , a(static_cast<uint8_t>(A)) {}

    /* Construct from red/green/blue, in the range of 0-255 per component. */
    color_t(const uint8_t R, const uint8_t G, const uint8_t B, const uint8_t A)
        : r(R)
        , g(G)
        , b(B)
        , a(A) {}

    /* Construct from HSV, in the range of 0-1.0 as floats. */
    color_t(const float hue, const float saturation, const float value) {
        std::tie(r, g, b, a) = color_from_hsv(hue, saturation, value);
    }

    /* Construct from an RGBA tuple */
    color_t(const std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>& c) {
        std::tie(r, g, b, a) = c;
    }

    /* You can add colors together, for example as a quick blend/lighten */
    color_t operator+(const color_t& other) {
        int red   = r + other.r;
        int green = g + other.g;
        int blue  = b + other.b;
        int alpha = a * other.a;
        if(red > 255)
            red = 255;
        if(green > 255)
            green = 255;
        if(blue > 255)
            blue = 255;
        if(alpha > 255)
            alpha = 255;
        return color_t(red, green, blue);
    }

    /* You can subtract colors */
    color_t operator-(const color_t& other) {
        int red   = r - other.r;
        int green = g - other.g;
        int blue  = b - other.b;
        int alpha = a * other.a;
        if(red < 0)
            red = 0;
        if(green < 0)
            green = 0;
        if(blue < 0)
            blue = 0;
        if(alpha > 255)
            alpha = 255;
        return color_t(red, green, blue);
    }

    /* You can multiply colors */
    color_t operator*(const color_t& other) {
        int red                  = r * other.r;
        int green                = g * other.g;
        int blue                 = b * other.b;
        int alpha                = a * other.a;
        static auto clip_channel = [](int& channel) {
            if(channel < 0) {
                channel = 0;
            } else if(channel > 255) {
                channel = 255;
            }
        };
        clip_channel(red);
        clip_channel(green);
        clip_channel(blue);
        clip_channel(alpha);
        return color_t(red, green, blue);
    }

    /* You can compare colors */
    bool operator==(const color_t& other) const {
        if(other.r == r && other.g == g && other.b == b && other.a == a) {
            return true;
        }
        return false;
    }

    template <class Archive>
    void serialize(Archive& archive) {
        archive(r, g, b, a);  // serialize things by passing them to the archive
    }
};


/**
 * @brief  Converts a color_t to an raylib Color
 *
 * @param col
 * @return Color
 */
inline Color color_to_rlcolor(const color_t& col) {
    return Color(col.r, col.g, col.b, col.a);
}

/* Converts a color_t to an RGBA tuple */
inline std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>
color_to_rgba(const color_t& col) {
    return std::make_tuple(col.r, col.g, col.b, col.a);
}

/* Converts a color_t to an HSV tuple */
extern std::tuple<float, float, float> color_to_hsv(const color_t& col);

/* Calculates the luminance of a color, and converts it to grey-scale. */
extern color_t greyscale(const color_t& col);

/* Darkens a color by the specified amount. */
color_t darken(const int& amount, const color_t& col);


/**
 * @brief Applies colored lighting effect; colors that don't exist remain dark.
 * Lights are from 0.0 to 1.0.
 *
 * @param col color
 * @param light
 * @return color_t
 */
color_t apply_colored_light(const color_t& col,
                            const std::tuple<float, float, float>& light);

/**
 * @brief Calculates an intermediate color on a linear RGB color ramp. Amount is
 * from 0.F to 1.F
 */
color_t lerp(const color_t& first, const color_t& second, float amount);

}  // namespace radl
