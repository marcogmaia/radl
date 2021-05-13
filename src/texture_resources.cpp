#include <unordered_map>
#include <stdexcept>
#include "texture_resources.hpp"
#include "texture.hpp"

namespace radl {

namespace texture_detail {

std::unordered_map<std::string, radl::texture> atlas;

}

void register_texture(const std::string& filename, const std::string& tag) {
    auto finder = texture_detail::atlas.find(tag);
    if(finder != texture_detail::atlas.end()) {
        throw std::runtime_error("Duplicate resource tag: " + tag);
    }
    texture_detail::atlas[tag] = radl::texture(filename);
}

Texture2D* get_texture(const std::string& tag) {
    auto finder = texture_detail::atlas.find(tag);
    if(finder == texture_detail::atlas.end()) {
        throw std::runtime_error("Unable to find resource tag: " + tag);
    }
    return finder->second.tex.get();
}

}  // namespace radl
