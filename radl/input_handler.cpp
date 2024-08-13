#include <algorithm>
#include <array>

#include "input_handler.hpp"

namespace radl {

namespace {

// bool window_focused = true;
std::array<bool, MOUSE_BUTTON_BACK + 1> mouse_button_pressed;
int mouse_x = 0;
int mouse_y = 0;

} // namespace

// bool is_window_focused() {
//     return radl::state::window_focused;
// }

// void set_window_focus_state(const bool& s) {
//     radl::state::window_focused = s;
// }

void reset_mouse_state() {
  std::fill(mouse_button_pressed.begin(), mouse_button_pressed.end(), false);
  mouse_x = 0;
  mouse_y = 0;
}

void set_mouse_state() {
  int idx = 0;
  for (auto &button : mouse_button_pressed) {
    button = IsMouseButtonDown(idx);
    ++idx;
  }
  mouse_x = GetMouseX();
  mouse_y = GetScreenHeight() - GetMouseY() - 1;
}

void set_mouse_state(float pos_x, float pos_y) {
  int idx = 0;
  for (auto &button : mouse_button_pressed) {
    button = IsMouseButtonDown(idx);
    ++idx;
  }
  if (pos_x < 0.0 || pos_y < 0.0) {
    return;
  }
  mouse_x = static_cast<int>(pos_x);
  mouse_y = static_cast<int>(pos_y);
}

void set_mouse_button_state(const MouseButton button, const bool state) {
  mouse_button_pressed[button] = state;
}

bool get_mouse_button_state(const MouseButton button) {
  return mouse_button_pressed[button];
}

void set_mouse_position(int x, int y) {
  mouse_x = x;
  mouse_y = y;
}

std::pair<int, int> get_mouse_position() {
  return std::make_pair(mouse_x, mouse_y);
}

} // namespace radl