#include <future>
#include <memory>

#include "path_finding.hpp"
#include "radl.hpp"
#include "texture.hpp"
#include "texture_resources.hpp"

namespace radl {

// Smart pointers makes it easy the use of RAII idiom
std::unique_ptr<radl::virtual_terminal> vterm;
std::unique_ptr<radl::gui_t> gui;

namespace main_detail {
bool use_root_console           = false;
bool taking_screenshot          = false;
std::string screenshot_filename = "";

Shader shader_mask;
}  // namespace main_detail

virtual_terminal& get_main_terminal() {
    return *vterm;
}

namespace {

template <typename T>
void init_common(const T& config, bool use_root_console = false) {
    int window_flags = FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT;
    if(config.fullscreen) {
        window_flags |= FLAG_FULLSCREEN_MODE;
    }
    SetConfigFlags(window_flags);
    main_detail::use_root_console = use_root_console;
    InitWindow(config.width, config.height, config.window_title.c_str());
    main_detail::shader_mask
        = LoadShader(nullptr, "../resources/shaders/mask.frag");
    // Register fonts after OpenGL init (InitWindow), and then resize the window
    // accordingly
    register_font_directory(config.font_path);
}

}  // namespace

void init(const config_simple& config) {
    init_common(config, true);

    bitmap_font* font               = get_font(config.root_font);
    auto& [font_width, font_height] = font->character_size;
    SetWindowSize(GetScreenWidth() * font_width,
                  GetScreenHeight() * font_height);

    radl::vterm
        = std::make_unique<virtual_terminal>(config.root_font, 0, 0, true);
    radl::vterm->resize_pixels(GetScreenWidth() * font_width,
                               GetScreenHeight() * font_height);
}

void terminate() {
    UnloadShader(main_detail::shader_mask);
}

void init(const config_simple_px& config) {
    init_common(config, true);

    radl::vterm
        = std::make_unique<virtual_terminal>(config.root_font, 0, 0, true);
    radl::vterm->resize_pixels(config.width, config.width);
}

void init(const config_advanced& config) {
    init_common(config, false);
    gui = std::make_unique<gui_t>(config.width, config.height);
}

// std::function<bool(event)> optional_event_hook = nullptr;
std::function<void()> optional_display_hook = nullptr;

void run(std::function<void(double)> on_tick) {
    reset_mouse_state();
    while(!WindowShouldClose()) {
        static auto clock        = std::chrono::steady_clock();
        static double delta_time = 0;
        auto start_time          = clock.now();
        set_mouse_state();

        // update
        on_tick(delta_time);

        // delta_time = GetFrameTime();

        // update
        if(main_detail::use_root_console) {
            radl::vterm->render();
        } else {
            radl::gui->render();
        }

        BeginDrawing();
        ClearBackground(BLACK);
        if(main_detail::use_root_console) {
            radl::vterm->draw();
        } else {
            radl::gui->draw();
        }
        DrawFPS(GetScreenWidth() - 100, 100);
        EndDrawing();
        delta_time
            = std::chrono::duration<double>(clock.now() - start_time).count();
    }

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


    // test_radl(gui.get_vterm(gui_handle));


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
