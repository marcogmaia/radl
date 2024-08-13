#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

// #include "boost/algorithm/string/split.hpp"
// #include "boost/algorithm/string/trim.hpp"
#include "filesystem.hpp"
#include "font_manager.hpp"
#include "texture_resources.hpp"
#include <nlohmann/json.hpp>

namespace radl {

namespace font_detail {

std::unordered_map<std::string, bitmap_font> atlas;

} // namespace font_detail

bitmap_font *get_font(const std::string &font_tag) {
  auto finder = font_detail::atlas.find(font_tag);
  if (finder == font_detail::atlas.end()) {
    throw std::runtime_error("Unable to locate bitmap font with tag " +
                             font_tag);
  }
  return &finder->second;
}

inline void check_for_duplicate_font(const std::string &tag) {
  auto finder = font_detail::atlas.find(tag);
  if (finder != font_detail::atlas.end()) {
    throw std::runtime_error("Attempting to insert duplicate font with tag " +
                             tag);
  }
}

inline void check_texture_exists(const std::string &texture_tag) {
  if (get_texture(texture_tag).id == 0) {
    throw std::runtime_error("No such texture resource: " + texture_tag);
  }
}

void register_font(const std::string &font_tag, const std::string &filename,
                   int tile_width, int tile_height) {
  const std::string texture_tag = "font_tex_" + filename;
  check_for_duplicate_font(font_tag);
  register_texture(filename, texture_tag);
  check_texture_exists(texture_tag);
  font_detail::atlas.emplace(std::make_pair(
      font_tag, bitmap_font(texture_tag, tile_width, tile_height)));
}

void RegisterFonts(const std::string &filepath) {
  const std::filesystem::path path{filepath};

  auto ifstream = std::ifstream{filepath};
  auto json = nlohmann::json::parse(ifstream);

  for (auto &entry : json.at("fonts")) {
    register_font(entry.at("name"),
                  (path.parent_path() / entry.at("file")).string(),
                  entry.at("width"), entry.at("height"));
  }
}

} // namespace radl
