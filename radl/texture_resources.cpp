#include <stdexcept>
#include <unordered_map>

#include "raylib.h"
#include "texture.hpp"
#include "texture_resources.hpp"

namespace radl {

namespace texture_detail {

std::unordered_map<std::string, radl::texture_t> atlas;

bool atlas_contains_tag(const std::string& tag) {
    auto itr = atlas.find(tag);
    return itr != atlas.end();
}

}  // namespace texture_detail

void register_texture(const std::string& filename, const std::string& tag) {
    if(texture_detail::atlas_contains_tag(tag)) {
        throw std::runtime_error("Duplicate resource tag: " + tag);
    }
    texture_detail::atlas.emplace(std::make_pair(tag, filename));
}

Texture2D get_texture(const std::string& tag) {
    auto finder = texture_detail::atlas.find(tag);
    if(finder == texture_detail::atlas.end()) {
        throw std::runtime_error("Unable to find resource tag: " + tag);
    }
    return finder->second.texture;
}


}  // namespace radl
