#include <memory>
#include "radl.hpp"
#include "texture.hpp"
#include "texture_resources.hpp"

namespace radl {

// Smart pointers makes it easy the use of RAII idiom
std::unique_ptr<radl::render_texture_t> main_texture;
std::unique_ptr<radl::virtual_terminal> vterm;
std::unique_ptr<radl::gui_t> gui;

namespace main_detail {
bool use_root_console;
bool taking_screenshot          = false;
std::string screenshot_filename = "";
}  // namespace main_detail

RenderTexture2D& get_window() {
    return main_texture->render_texture;
}

void init(const config_simple& config) {
    int window_flags = FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT;
    if(config.fullscreen) {
        window_flags |= FLAG_FULLSCREEN_MODE;
    }
    SetConfigFlags(window_flags);
    InitWindow(config.width, config.height, config.window_title.c_str());
    // Register fonts after OpenGL init (InitWindow), and then resize the window
    // accordingly
    register_font_directory(config.font_path);
    bitmap_font* font               = get_font(config.root_font);
    auto& [font_width, font_height] = font->character_size;
    SetWindowSize(GetScreenWidth() * font_width,
                  GetScreenHeight() * font_height);

    main_detail::use_root_console = true;
    main_texture = std::make_unique<render_texture_t>(1920, 1080);

    int texture_width  = main_texture->render_texture.texture.width;
    int texture_height = main_texture->render_texture.texture.height;
    radl::vterm = std::make_unique<virtual_terminal>(config.root_font, 0, 0);
    radl::vterm->resize_pixels(texture_width, texture_height);
}


void init(const config_simple_px& config) {
    int window_flags = FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT;
    if(config.fullscreen) {
        window_flags |= FLAG_FULLSCREEN_MODE;
    }
    SetConfigFlags(window_flags);
    InitWindow(config.width_px, config.height_px, config.window_title.c_str());

    main_detail::use_root_console = true;
    main_texture = std::make_unique<render_texture_t>(1920, 1080);

    int texture_width  = main_texture->render_texture.texture.width;
    int texture_height = main_texture->render_texture.texture.height;
    radl::vterm = std::make_unique<virtual_terminal>(config.root_font, 0, 0);
    radl::vterm->resize_pixels(texture_width, texture_height);
}

void init(const config_advanced& config) {
    int window_flags = FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT;
    if(config.fullscreen) {
        window_flags |= FLAG_FULLSCREEN_MODE;
    }
    SetConfigFlags(window_flags);
    InitWindow(config.width_px, config.height_px, config.window_title.c_str());
    // Doesn't work before InitWindow because GL isn't initialized...
    // It needs to registers the font as a texture, and OpenGL needs to be
    // initialized to store the texture
    register_font_directory(config.font_path);

    main_texture = std::make_unique<render_texture_t>(1920, 1080);

    main_detail::use_root_console = false;
    gui = std::make_unique<gui_t>(config.width_px, config.height_px);
}

// std::function<bool(sf::Event)> optional_event_hook = nullptr;
std::function<void()> optional_display_hook = nullptr;

void run(std::function<void(double)> on_tick) {
    // reset_mouse_state();

    // double duration_ms = 0.0;
    // while(main_texture->isOpen()) {
    //     clock_t start_time = clock();

    //     sf::Event event;
    //     while(main_texture->pollEvent(event)) {
    //         bool handle_events = true;
    //         if(optional_event_hook) {
    //             handle_events = optional_event_hook(event);
    //         }
    //         if(handle_events) {
    //             if(event.type == sf::Event::Closed) {
    //                 main_texture->close();
    //             } else if(event.type == sf::Event::Resized) {
    //                 main_texture->setView(sf::View(sf::FloatRect(
    //                     0.f, 0.f, static_cast<float>(event.size.width),
    //                     static_cast<float>(event.size.height))));
    //                 if(main_detail::use_root_console)
    //                     console->resize_pixels(event.size.width,
    //                                            event.size.height);
    //                 if(gui)
    //                     gui->on_resize(event.size.width, event.size.height);
    //             } else if(event.type == sf::Event::LostFocus) {
    //                 set_window_focus_state(false);
    //             } else if(event.type == sf::Event::GainedFocus) {
    //                 set_window_focus_state(true);
    //             } else if(event.type == sf::Event::MouseButtonPressed) {
    //                 set_mouse_button_state(event.mouseButton.button, true);
    //             } else if(event.type == sf::Event::MouseButtonReleased) {
    //                 set_mouse_button_state(event.mouseButton.button, false);
    //             } else if(event.type == sf::Event::MouseMoved) {
    //                 set_mouse_position(event.mouseMove.x, event.mouseMove.y);
    //             } else if(event.type == sf::Event::KeyPressed) {
    //                 enqueue_key_pressed(event);
    //             } else if(event.type == sf::Event::MouseWheelMoved) {
    //                 if(event.mouseWheel.delta < 0) {
    //                     set_mouse_button_state(button::WHEEL_UP, true);
    //                 } else if(event.mouseWheel.delta > 0) {
    //                     set_mouse_button_state(button::WHEEL_DOWN, true);
    //                 }
    //             }
    //         }
    //     }

    //     main_texture->clear();
    //     // if (main_detail::use_root_console) console->clear();

    //     on_tick(duration_ms);

    //     if(main_detail::use_root_console) {
    //         console->render(*main_texture);
    //     } else {
    //         gui->render(*main_texture);
    //     }

    //     if(optional_display_hook) {
    //         main_texture->pushGLStates();
    //         main_texture->resetGLStates();
    //         optional_display_hook();
    //         main_texture->popGLStates();
    //     }
    //     main_texture->display();
    //     if(main_detail::taking_screenshot) {
    //         sf::Texture tex;
    //         tex.update(*rltk::get_window());
    //         sf::Image screen = tex.copyToImage();

    //         screen.saveToFile(main_detail::screenshot_filename);
    //         main_detail::screenshot_filename = "";
    //         main_detail::taking_screenshot   = false;
    //     }

    //     duration_ms = ((clock() - start_time) * 1000.0) / CLOCKS_PER_SEC;
    // }
}

void request_screenshot(const std::string& filename) {
    main_detail::taking_screenshot   = true;
    main_detail::screenshot_filename = filename;
}

}  // namespace radl
