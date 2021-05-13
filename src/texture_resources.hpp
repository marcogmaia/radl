#pragma once

#include <string>
#include "raylib.h"

namespace radl {

void register_texture(const std::string& filename, const std::string& tag);
Texture* get_texture(const std::string& tag);

}  // namespace radl
