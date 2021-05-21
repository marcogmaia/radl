#include "layer_t.hpp"
// #include "input_handler.hpp"

namespace radl {

void layer_t::make_owner_draw_backing() {
    if(backing.id == 0) {
        backing = LoadRenderTexture(w, h);
    }
}

/**
 * @brief Resizes the layer, by resizing the underlying virtual_terminal.
 *
 * @note This function is called by gui_t::on_resize
 *
 * @param width
 * @param height
 */
void layer_t::on_resize(const int width, const int height) {
    if(!resize_func) {
        return;
    }
    resize_func(this, width, height);
    if(vterm && vterm->visible) {
        vterm->set_offset(x, y);
        vterm->resize_pixels(w, h);
        vterm->dirty = true;
    } else {
        make_owner_draw_backing();
    }
}

void layer_t::render() {
    if(vterm) {
        // render start events
        for(auto& [handle, control] : controls) {
            if(control->on_render_start) {
                control->on_render_start(control.get());
            }
        }

        // auto [mouse_x, mouse_y] = get_mouse_position();

        // if mouse is inside layer: handle mouse functions
        // if(mouse_x >= x && mouse_x <= (x + w) && mouse_y >= y
        //    && mouse_y <= (y + h)) {
        //     // Mouse over in here is possible.
        //     auto font_dimensions = console->get_font_size();
        //     const int terminal_x = (mouse_x - x) / font_dimensions.first;
        //     const int terminal_y = (mouse_y - y) / font_dimensions.second;

        //     for(auto it = controls.begin(); it != controls.end(); ++it) {
        //         // Mouse over
        //         if(it->second->mouse_in_control(terminal_x, terminal_y)) {
        //             if(it->second->on_mouse_over) {
        //                 auto callfunc = it->second->on_mouse_over;
        //                 callfunc(it->second.get(), terminal_x, terminal_y);
        //             }

        //             // Mouse down and up
        //             if(get_mouse_button_state(button::LEFT)
        //                && it->second->on_mouse_down) {
        //                 auto callfunc = it->second->on_mouse_down;
        //                 callfunc(it->second.get(), terminal_x, terminal_y);
        //             }
        //             if(!get_mouse_button_state(button::LEFT)
        //                && it->second->on_mouse_up) {
        //                 auto callfunc = it->second->on_mouse_up;
        //                 callfunc(it->second.get(), terminal_x, terminal_y);
        //             }
        //         }
        //     }
        // }

        // for(auto it = controls.begin(); it != controls.end(); ++it) {
        //     it->second->render(console.get());
        // }
        for(auto& [handle, control] : controls) {
            control->render(vterm.get());
        }
        vterm->render();
        //     }
        //     console->render(render_texture);
        // } else if(sconsole) {
        //     sconsole->render(render_texture);
        // } else {
        // if(!backing)
        //     make_owner_draw_backing();
        // backing->clear(sf::Color(0, 0, 0, 0));
        // owner_draw_func(this, *backing);
        // backing->display();
        // sf::Sprite compositor(backing->getTexture());
        // compositor.move(static_cast<float>(x), static_cast<float>(y));
        // window.draw(sf::Sprite(compositor));
    } else if(svterm) {
        svterm->render();
    } else {  // has backing
        // if backing doesn't exist, create one
        make_owner_draw_backing();
        BeginTextureMode(backing);
        ClearBackground(BLANK);
        owner_draw_func(this, backing);
        EndTextureMode();
    }
}

void layer_t::draw() {
    if(vterm) {
        vterm->draw();
    } else if(svterm) {
        svterm->draw();
    }
    // TODO owner layer
}

void layer_t::clear() {
    if(vterm) {
        vterm->clear();
    } else if(svterm) {
        svterm->clear();
    }
    // TODO owner layer
}

void resize_fullscreen(radl::layer_t* l, int w, int h) {
    // Use the whole window
    l->w = w;
    l->h = h;
}

}  // namespace radl
