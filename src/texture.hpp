/*
 * Provides RAII wrapper for a texture.
 */

#pragma once

#include <memory>
#include <string>
#include <stdexcept>
#include "raylib.h"

namespace radl {

struct texture {
    std::unique_ptr<Texture2D> tex;
    texture() {}
    inline texture(const std::string& filename) {
        tex  = std::make_unique<Texture2D>();
        *tex = LoadTexture(filename.c_str());
        if(tex->id == 0) {
            throw std::runtime_error("Unable to load texture from: "
                                     + filename);
        }
    }
};

}  // namespace radl
