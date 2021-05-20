#include "gui.hpp"
#include <stdexcept>
#include <utility>
#include <algorithm>

namespace radl {

namespace gui_detail {

std::vector<std::pair<int, layer_t*>> render_order;

}

void gui_t::on_resize(const int w, const int h) {
    screen_width  = w;
    screen_height = h;

    for(auto& [key, layer] : layers) {
        layer.on_resize(w, h);
    }
}

void gui_t::render() {
    for(auto& [handle, layer] : gui_detail::render_order) {
        layer->render();
    }
}

void gui_t::add_layer(const int handle, const int X, const int Y, const int W,
                      const int H, std::string font_name,
                      std::function<void(layer_t*, int, int)> resize_fun,
                      bool has_background, int order) {
    check_handle_uniqueness(handle);
    layers.emplace(handle,
                   layer_t(X, Y, W, H, font_name, resize_fun, has_background));
    if(order == -1) {
        order = render_order;
        ++render_order;
    }
    gui_detail::render_order.push_back(
        std::make_pair(order, get_layer(handle)));
    std::sort(gui_detail::render_order.begin(), gui_detail::render_order.end(),
              [](std::pair<int, layer_t*> a, std::pair<int, layer_t*> b) {
                  return a.first < b.first;
              });
}

void gui_t::add_sparse_layer(const int handle, const int X, const int Y,
                             const int W, const int H, std::string font_name,
                             std::function<void(layer_t*, int, int)> resize_fun,
                             int order) {
    check_handle_uniqueness(handle);
    layers.emplace(handle, layer_t{true, X, Y, W, H, font_name, resize_fun});
    if(order == -1) {
        order = render_order;
        ++render_order;
    }
    gui_detail::render_order.push_back(
        std::make_pair(order, get_layer(handle)));
    std::sort(gui_detail::render_order.begin(), gui_detail::render_order.end(),
              [](std::pair<int, layer_t*> a, std::pair<int, layer_t*> b) {
                  return a.first < b.first;
              });
}

void gui_t::add_owner_layer(
    const int handle, const int X, const int Y, const int W, const int H,
    std::function<void(layer_t*, int, int)> resize_fun,
    std::function<void(layer_t*, RenderTexture2D&)> owner_draw_fun, int order) {
    check_handle_uniqueness(handle);
    layers.emplace(handle, layer_t{X, Y, W, H, resize_fun, owner_draw_fun});
    if(order == -1) {
        order = render_order;
        ++render_order;
    }
    gui_detail::render_order.push_back(
        std::make_pair(order, get_layer(handle)));
    std::sort(gui_detail::render_order.begin(), gui_detail::render_order.end(),
              [](std::pair<int, layer_t*> a, std::pair<int, layer_t*> b) {
                  return a.first < b.first;
              });
}

void gui_t::delete_layer(const int handle) {
    gui_detail::render_order.erase(
        std::remove_if(gui_detail::render_order.begin(),
                       gui_detail::render_order.end(),
                       [&handle, this](std::pair<int, layer_t*> a) {
                           if(a.second == this->get_layer(handle))
                               return true;
                           return false;
                       }),
        gui_detail::render_order.end());
    layers.erase(handle);
}

layer_t* gui_t::get_layer(const int handle) {
    auto finder = layers.find(handle);
    if(finder == layers.end())
        throw std::runtime_error("Unknown layer handle: "
                                 + std::to_string(handle));
    return &(finder->second);
}

}  // namespace radl
