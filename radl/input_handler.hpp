#pragma once
/*
 * Copyright (c) 2016-Present, Bracket Productions.
 * Licensed under the MIT license - see LICENSE file.
 *
 * Class for providing a simple input interface.
 * Provides functions that can be queried for the current state of
 * the input system.
 *
 */

#include <raylib.h>
#include <utility>

namespace radl {

/* Helper constants to represent mouse buttons */
namespace button {

constexpr auto LEFT = MOUSE_BUTTON_LEFT;
constexpr auto RIGHT = MOUSE_BUTTON_RIGHT;
constexpr auto MIDDLE = MOUSE_BUTTON_MIDDLE;
constexpr auto SIDE = MOUSE_BUTTON_SIDE;
constexpr auto EXTRA = MOUSE_BUTTON_EXTRA;
constexpr auto FORWARD = MOUSE_BUTTON_FORWARD;
constexpr auto BACK = MOUSE_BUTTON_BACK;

} // namespace button

/* Does the game window currently have focus? You might want to pause if it
 * doesn't. */
bool is_window_focused();

/* Setter function for window focus */
void set_window_focus_state(const bool &s);

/* Mouse state reset: clears all mouse state */
void reset_mouse_state();

/**
 * @brief Set all mouse buttons state
 */
void set_mouse_state();
void set_mouse_state(float pos_x, float pos_y);

/* Update the stored mouse position. Does not actually move the mouse. */
void set_mouse_position(int x, int y);

std::pair<int, int> get_mouse_position();

/* Mouse button state */
void set_mouse_button_state(const MouseButton button, const bool state);
bool get_mouse_button_state(const MouseButton button);

/* Keyboard queue */
// extern void enqueue_key_pressed(sf::Event& event);

// struct key_pressed_t : base_message_t {
// public:
//     key_pressed_t() {}
//     key_pressed_t(sf::Event ev)
//         : event(ev) {}
//     sf::Event event;
// };

} // namespace radl
