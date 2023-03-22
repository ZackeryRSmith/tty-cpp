#include <iostream>
#include "../include/tty-cpp.hpp"

std::string key_name(Key key) {
    switch (key) {
        case Key::ESC: return "ESC";
        case Key::DEL: return "DELETE";
        case Key::ENTER: return "ENTER";
        case Key::UP_ARROW: return "UP ARROW";
        case Key::DOWN_ARROW: return "DOWN ARROW";
        case Key::LEFT_ARROW: return "LEFT ARROW";
        case Key::RIGHT_ARROW: return "RIGHT ARROW";
        default:
            if (static_cast<int>(key) >= static_cast<int>(Key::CTRL_A) && static_cast<int>(key) <= static_cast<int>(Key::CTRL_Z)) {
                return "CTRL+" + std::string(1, 'A' + (static_cast<int>(key) - static_cast<int>(Key::CTRL_A)));
            } else {
                return std::string(1, static_cast<char>(key));
            }
    }
}

int main() {
    RawModeGuard raw_mode_guard;
    while (true) {
        Key pressed_key = Term::getkey();

        std::cout << "Key pressed: " << key_name(pressed_key) << std::endl;
        if (pressed_key == Key::CTRL_C)
            break; // Exit the loop
    }

    return 0;
}
