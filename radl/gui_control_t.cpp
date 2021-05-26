#include <algorithm>
#include <sstream>

#include "gui_control_t.hpp"
namespace radl {

void gui_static_text_t::render(virtual_terminal* vterm) {
    vterm->print(x, y, text, foreground, background);
}

void gui_border_box_t::render(virtual_terminal* vterm) {
    vterm->box(foreground, background, is_double);
}

void gui_checkbox_t::render(virtual_terminal* vterm) {
    vterm->set_char(x, y, vchar_t{'[', foreground, background});
    if(checked) {
        vterm->set_char(x + 1, y, vchar_t{'X', foreground, background});
    } else {
        vterm->set_char(x + 1, y, vchar_t{' ', foreground, background});
    }
    vterm->print(x + 2, y, "] " + label, foreground, background);
}

void gui_radiobuttons_t::render(virtual_terminal* vterm) {
    vterm->print(x, y, caption, foreground, background);
    int current_y = y + 1;
    for(const radio& r : options) {
        vterm->set_char(x, current_y, vchar_t{'(', foreground, background});
        if(r.checked) {
            vterm->set_char(x + 1, current_y,
                            vchar_t{'*', foreground, background});
        } else {
            vterm->set_char(x + 1, current_y,
                            vchar_t{' ', foreground, background});
        }
        vterm->print(x + 2, current_y, ") " + r.label, foreground, background);
        ++current_y;
    }
}

void gui_hbar_t::render(virtual_terminal* vterm) {
    float fullness     = (float)(value - min) / (float)max;
    float full_w_f     = fullness * (float)w;
    std::size_t full_w = static_cast<std::size_t>(full_w_f);

    std::stringstream ss;
    for(std::size_t i = 0; i < w; ++i) {
        ss << ' ';
    }
    std::string s = ss.str();

    std::stringstream display;
    display << prefix << value << "/" << max;
    std::string tmp = display.str();
    const int start = static_cast<int>((w / 2) - (tmp.size() / 2));
    for(std::size_t i = 0; i < std::min(tmp.size(), w); ++i) {
        s[i + start] = tmp[i];
    }

    for(std::size_t i = 0; i < w; ++i) {
        const float pct = (float)i / (float)w;
        if(i <= full_w) {
            vterm->set_char(
                x + i, y,
                vchar_t{s[i], text_color, lerp(full_start, full_end, pct)});
        } else {
            vterm->set_char(
                x + i, y,
                vchar_t{s[i], text_color, lerp(empty_start, empty_end, pct)});
        }
    }
}

void gui_vbar_t::render(virtual_terminal* vterm) {
    float fullness     = (float)(value - min) / (float)max;
    float full_h_f     = fullness * (float)h;
    std::size_t full_h = static_cast<std::size_t>(full_h_f);

    std::stringstream ss;
    for(std::size_t i = 0; i < h; ++i) {
        ss << ' ';
    }
    std::string s = ss.str();

    std::stringstream display;
    display << prefix << value << "/" << max;
    std::string tmp = display.str();
    const int start = static_cast<int>((h / 2) - (tmp.size() / 2));
    for(std::size_t i = 0; i < std::min(tmp.size(), h); ++i) {
        s[i + start] = tmp[i];
    }

    for(std::size_t i = 0; i < h; ++i) {
        const float pct = (float)i / (float)h;
        if(i <= full_h) {
            vterm->set_char(
                x, y + i,
                vchar_t{s[i], text_color, lerp(full_start, full_end, pct)});
        } else {
            vterm->set_char(
                x, y + i,
                vchar_t{s[i], text_color, lerp(empty_start, empty_end, pct)});
        }
    }
}

void gui_listbox_t::render(virtual_terminal* vterm) {
    vterm->box(x, y, w + 3, static_cast<int>(items.size()) + 1, caption_fg,
               caption_bg, false);
    vterm->print(x + 3, y, caption, caption_fg, caption_bg);
    vterm->set_char(x + 1, y, vchar_t{180, caption_fg, caption_bg});
    vterm->set_char(x + 2, y, vchar_t{' ', caption_fg, caption_bg});
    vterm->set_char(x + w, y, vchar_t{' ', caption_fg, caption_bg});
    vterm->set_char(x + w + 1, y, vchar_t{195, caption_fg, caption_bg});
    int current_y = y + 1;
    for(const list_item& item : items) {
        if(item.value == selected_value) {
            vterm->print(x + 2, current_y, item.label, selected_fg,
                         selected_bg);
        } else {
            vterm->print(x + 2, current_y, item.label, item_fg, item_bg);
        }
        ++current_y;
    }
}

}  // namespace radl