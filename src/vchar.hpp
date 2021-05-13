#pragma once

#include "color_t.hpp"

namespace radl {

/*
 * Represents a character on a virtual terminal. (virtual char)
 */
struct vchar {
    uint32_t glyph;
    color_t foreground;
    color_t background;

    vchar() {}
    vchar(const uint32_t& g, const color_t& f, const color_t& b)
        : glyph(g)
        , foreground(f)
        , background(b) {}
    vchar(const int& g, const color_t& f, const color_t& b)
        : glyph(static_cast<uint32_t>(g))
        , foreground(f)
        , background(b) {}
    vchar(const uint32_t& gly, const uint8_t& fr, const uint8_t& fg,
          const uint8_t& fb, const uint8_t& br, const uint8_t& bg,
          const uint8_t& bb)
        : glyph(gly)
        , foreground(color_t{fr, fg, fb})
        , background(color_t{br, bg, bb}) {}
    vchar(const uint32_t& gly, const uint8_t& fr, const uint8_t& fg,
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

}  // namespace radl
