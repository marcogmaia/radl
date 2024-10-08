#pragma once

#include <functional>
#include <string>
#include <utility>

#include "gui.hpp"
#include "raylib.h"
#include "virtual_terminal.hpp"

namespace radl {

/*
 * Defines a simple configuration to get most people started. 16x16 font, window
 * size 1024x768.
 */
struct config_simple_px {
  const std::string font_path;
  const int width;
  const int height;
  const std::string window_title;
  const std::string root_font;
  const bool fullscreen;

  config_simple_px(std::string fonts_file_path, const int width = 1024,
                   const int height = 768, std::string title = "RADL",
                   std::string font = "16x16", bool full_screen = false)
      : font_path(std::move(fonts_file_path)), width(width), height(height),
        window_title(std::move(title)), root_font(std::move(font)),
        fullscreen(full_screen) {}
};

/*
 * Defines a simple configuration to get most people started. 8x8 font, window
 * size 128x96 (which happens to be 1024x768)
 */
struct config_simple {
  const std::string font_path;
  const int width;
  const int height;
  const std::string window_title;
  const std::string root_font;
  const bool fullscreen;

  config_simple(std::string fonts_file_path, const int width_term = 64,
                const int height_term = 48, std::string title = "RADL",
                std::string font = "16x16", bool full_screen = false)
      : font_path(std::move(fonts_file_path)), width(width_term),
        height(height_term), window_title(std::move(title)),
        root_font(std::move(font)), fullscreen(full_screen) {}
};

/*
 * Defines an advanced configuration. No root console, so it is designed for the
 * times you want to build your own GUI.
 */
struct config_advanced {
  const std::string font_path;
  const int width;
  const int height;
  const std::string window_title;
  const bool fullscreen;

  config_advanced(std::string fonts_file_path, const int width = 1024,
                  const int height = 768, std::string title = "RADL",
                  bool full_screen = false)
      : font_path(std::move(fonts_file_path)), width(width), height(height),
        window_title(std::move(title)), fullscreen(full_screen) {}
};

/*
 * Bootstrap the system with a simple configuration, specified in pixels.
 * This variant uses terminal coordinates - so specify 80x40 as size and it will
 * scale with the terminal size.
 */
void init(const config_simple &config);

/*
 * Bootstrap the system with a simple configuration, specified in pixels.
 * This variant uses screen coordinates.
 */
void init(const config_simple_px &config);

/*
 * Bootstrap the system with a simple configuration, specified in pixels.
 * This variant doesn't set up much automatically; it's designed for the times
 * you want to define your own GUI.
 */
void init(const config_advanced &config);

/**
 * @brief clear all initialized configs and parameters
 *
 */
void terminate();

/*
 * The main run loop. Calls on_tick each frame. Window can be initially defined
 * with width/height/title, but these have sane defaults to get you started.
 */
void run(const std::function<void(double)> &on_tick);

/*
 * For rendering to the console
 */
extern std::unique_ptr<virtual_terminal> vterm;

/*
 * In case you want to do some raw stuff yourself, this provides a pointer to
 * the main render texture.
 */
virtual_terminal &get_main_terminal();

/**
 * @brief Draw the entire texture to the screen
 *
 * @param render_texture
 */
inline void draw(RenderTexture2D &render_texture) {
  // NOTE: Render texture must be y-flipped due to default OpenGL
  // coordinates (left-bottom)
  DrawTextureRec(render_texture.texture,
                 Rectangle{0.F, 0.F,
                           static_cast<float>(render_texture.texture.width),
                           static_cast<float>(-render_texture.texture.height)},
                 Vector2{0.F, 0.F}, WHITE);
}

/*
 * For GUI manipulation
 */
extern std::unique_ptr<gui_t> gui;

/*
 * Convenience function to quickly get a GUI layer
 */
inline layer_t &layer(const int handle) { return *gui->get_layer(handle); }

inline virtual_terminal &get_vterm(const int handle) {
  return *gui->get_layer(handle)->vterm.get();
}

inline virtual_terminal_sparse &get_svterm(const int handle) {
  return *gui->get_layer(handle)->svterm.get();
}

/* Request a screenshot */
void request_screenshot(const std::string &filename);

/* Lifecycle hooks, for example to integrate ImGui with your application. */
// extern std::function<bool(/* event_params? */)> optional_event_hook;
extern std::function<void()> optional_display_hook;

} // namespace radl
