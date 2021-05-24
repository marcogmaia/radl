#include "input_handler.hpp"

#include <algorithm>
#include <array>

namespace radl {

namespace state {

bool game_running = false;
// bool window_focused = true;
std::array<bool, MOUSE_BUTTON_MAX> mouse_button_pressed;
int mouse_x = 0;
int mouse_y = 0;

}  // namespace state

// bool is_window_focused() {
//     return radl::state::window_focused;
// }

// void set_window_focus_state(const bool& s) {
//     radl::state::window_focused = s;
// }

bool is_game_running() {
    return state::game_running;
}

void set_game_running_state(bool running) {
    state::game_running = running;
}

void reset_mouse_state() {
    std::fill(radl::state::mouse_button_pressed.begin(),
              radl::state::mouse_button_pressed.end(), false);
    radl::state::mouse_x = 0;
    radl::state::mouse_y = 0;
}

void set_mouse_state() {
    int idx = 0;
    for(auto& button : radl::state::mouse_button_pressed) {
        button = IsMouseButtonDown(idx);
        ++idx;
    }
    radl::state::mouse_x = GetMouseX();
    radl::state::mouse_y = GetMouseY();
}

void set_mouse_state(float pos_x, float pos_y) {
    int idx = 0;
    for(auto& button : radl::state::mouse_button_pressed) {
        button = IsMouseButtonDown(idx);
        ++idx;
    }
    if(pos_x < 0.0 || pos_y < 0.0) {
        return;
    }
    radl::state::mouse_x = static_cast<int>(pos_x);
    radl::state::mouse_y = static_cast<int>(pos_y);
}

void set_mouse_button_state(const MouseButton button, const bool state) {
    radl::state::mouse_button_pressed[button] = state;
}

bool get_mouse_button_state(const MouseButton button) {
    return radl::state::mouse_button_pressed[button];
}

void set_mouse_position(const int x, const int y) {
    radl::state::mouse_x = x;
    radl::state::mouse_y = y;
}

std::pair<int, int> get_mouse_position() {
    return std::make_pair(radl::state::mouse_x, radl::state::mouse_y);
}

// void enqueue_key_pressed(sf::Event& event) {
//     emit(key_pressed_t{event});
// }

}  // namespace radl