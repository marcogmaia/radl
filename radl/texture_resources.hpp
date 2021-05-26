#pragma once

#include <string>

#include "raylib.h"

namespace radl {

void register_texture(const std::string& filename, const std::string& tag);
Texture2D get_texture(const std::string& tag);

}  // namespace radl
