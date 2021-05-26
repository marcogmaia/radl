/*
 * Provides RAII wrapper for a texture and render_texture.
 */

#pragma once

#include <stdexcept>
#include <string>

#include "raylib.h"

namespace radl {

/**
 * @brief Class wrapper to use RAII idiom
 *
 */
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
        // checks if texture is valid is done internal to raylib
        UnloadTexture(texture);
    }
};

/**
 * @brief Class wrapper to use RAII idiom
 *
 */
struct render_texture_t {
    RenderTexture2D render_texture = {0};

    inline render_texture_t(const int width, const int height) {
        render_texture = LoadRenderTexture(width, height);
        if(render_texture.id == 0) {
            throw std::runtime_error("Unable to create Framebuffer object");
        }
    }

    render_texture_t(const render_texture_t&) = delete;
    render_texture_t& operator=(const render_texture_t&) = delete;

    inline ~render_texture_t() {
        // checks if RenderTexture2D is valid is done inside raylib internals
        UnloadRenderTexture(render_texture);
    }

    /**
     * @brief Clears the RenderTexture
     *
     * @param color optional color to clear the texture
     */
    inline void clear(const Color& color = BLANK) {
        BeginTextureMode(render_texture);
        ClearBackground(color);
        EndTextureMode();
    }
};

inline void texture_clear(RenderTexture2D& render_texture,
                          const Color& color = BLANK) {
    BeginTextureMode(render_texture);
    ClearBackground(color);
    EndTextureMode();
}

}  // namespace radl
