#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>

#include "filesystem.hpp"
#include "font_manager.hpp"
#include "texture_resources.hpp"

namespace radl {

namespace font_detail {

std::unordered_map<std::string, bitmap_font> atlas;

}  // namespace font_detail

bitmap_font* get_font(const std::string& font_tag) {
    auto finder = font_detail::atlas.find(font_tag);
    if(finder == font_detail::atlas.end()) {
        throw std::runtime_error("Unable to locate bitmap font with tag "
                                 + font_tag);
    }
    return &finder->second;
}

inline void check_for_duplicate_font(const std::string& tag) {
    auto finder = font_detail::atlas.find(tag);
    if(finder != font_detail::atlas.end()) {
        throw std::runtime_error("Attempting to insert duplicate font with tag "
                                 + tag);
    }
}

inline void check_texture_exists(const std::string& texture_tag) {
    if(get_texture(texture_tag).id == 0) {
        throw std::runtime_error("No such texture resource: " + texture_tag);
    }
}

void register_font(const std::string& font_tag, const std::string& filename,
                   int tile_width, int tile_height) {
    const std::string texture_tag = "font_tex_" + filename;
    check_for_duplicate_font(font_tag);
    register_texture(filename, texture_tag);
    check_texture_exists(texture_tag);
    font_detail::atlas.emplace(std::make_pair(
        font_tag, bitmap_font(texture_tag, tile_width, tile_height)));
}

/**
 * @brief register the font directory with the file fonts.txt in the proper
 * CSV format:
 * "tag,filename,char_width,char_height"
 *
 * @warning Doesn't work before InitWindow because GL isn't initialized...
 * It needs to registers the font as a texture, and OpenGL needs to be
 * initialized to store the texture
 *
 * @param path path where the fonts.txt file is located
 */
void register_font_directory(std::string path) {
    boost::trim_right_if(path, boost::is_any_of(" /"));
    if(!exists(path)) {
        throw std::runtime_error("Font directory does not exist.");
    }
    const std::string info_file = path + "/fonts.txt";
    if(!exists(info_file)) {
        throw std::runtime_error("No fonts.txt file in font directory.");
    }

    std::ifstream f(info_file);
    std::string line;
    while(getline(f, line)) {
        std::vector<std::string> csv_fields;
        boost::split(csv_fields, line, boost::is_any_of(","));
        if(csv_fields.size() == 4) {
            register_font(csv_fields[0], path + "/" + csv_fields[1],
                          std::stoi(csv_fields[2]), std::stoi(csv_fields[3]));
        }
    }

    // TODO make use of json files
    /*
    ptree font_tree;
    read_json(info_file, font_tree);

    ptree::const_iterator end = font_tree.get_child("fonts").end();
    for (ptree::const_iterator it = font_tree.get_child("fonts").begin(); it !=
    end; ++it) { const std::string font_name = it->first; const std::string
    font_tree_path = "fonts." + font_name + "."; const std::string font_file =
    font_tree.get<std::string>(font_tree_path + "file"); const int width =
    font_tree.get<int>(font_tree_path + "width"); const int height =
    font_tree.get<int>(font_tree_path + "height");

        register_font(font_name, path + "/" + font_file, width, height);
    }*/
}

}  // namespace radl
