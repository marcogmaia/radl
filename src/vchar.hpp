#pragma once

#include "color_t.hpp"

namespace radl {

/*
 * Represents a character on a virtual terminal. (virtual char)
 */
struct vchar_t {
    uint32_t glyph;
    color_t foreground;
    color_t background;

    vchar_t() {}
    vchar_t(const uint32_t& g, const color_t& f, const color_t& b)
        : glyph(g)
        , foreground(f)
        , background(b) {}
    vchar_t(const int& g, const color_t& f, const color_t& b)
        : glyph(static_cast<uint32_t>(g))
        , foreground(f)
        , background(b) {}
    vchar_t(const uint32_t& gly, const uint8_t& fr, const uint8_t& fg,
            const uint8_t& fb, const uint8_t& br, const uint8_t& bg,
            const uint8_t& bb)
        : glyph(gly)
        , foreground(color_t{fr, fg, fb})
        , background(color_t{br, bg, bb}) {}
    vchar_t(const uint32_t& gly, const uint8_t& fr, const uint8_t& fg,
            const uint8_t& fa, const uint8_t& fb, const uint8_t& br,
            const uint8_t& bg, const uint8_t& bb, const uint8_t& ba)
        : glyph(gly)
        , foreground(color_t{fr, fg, fb, fa})
        , background(color_t{br, bg, bb, ba}) {}

    template <class Archive>
    void serialize(Archive& archive) {
        archive(glyph, foreground,
                background);  // serialize things by passing them to the archive
    }
};

/*
 * Represents a character on a sparse virtual terminal.
 */
struct svchar_t {
    uint32_t glyph;      // Glyph to render
    color_t foreground;  // Foreground color
    // Provided as floats to allow for partial character movement/sliding
    float x   = 0.0f;
    float y   = 0.0f;
    int angle = 0;  // Rotation angle in degrees, defaults to 0 - not rotated
    unsigned char opacity = 255;
    bool has_background   = false;
    color_t background;  // If provided, a background is drawn

    svchar_t() = default;

    svchar_t(const uint32_t Glyph, const color_t fg, const float X,
             const float Y, const int Angle = 0)
        : glyph(Glyph)
        , foreground(fg)
        , x(X)
        , y(Y)
        , angle{Angle} {}
};

}  // namespace radl
