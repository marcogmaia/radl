/*
 * Provides RAII wrapper for a texture.
 */

#pragma once

#include <string>
#include <stdexcept>
#include "raylib.h"

namespace radl {

struct texture_t {
    Texture2D texture = {0};
    inline texture_t(const std::string& filename) {
        texture = LoadTexture(filename.c_str());
        if(texture.id == 0) {
            throw std::runtime_error("Unable to load texture from: "
                                     + filename);
        }
    }
    inline ~texture_t() {
        UnloadTexture(texture);
    }
};

}  // namespace radl
