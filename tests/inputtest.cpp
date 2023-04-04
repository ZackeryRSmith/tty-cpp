#include <iostream>
#include "../include/tty-cpp.hpp"

std::string key_name(Key key) {
    switch (key) {
        case Key::ESC: return "ESC";
        case Key::DEL: return "DELETE";
        case Key::SPACE: return "SPACE";
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
    
    std::cout << Term::style(Term::Style::BOLD) << "Running tty-cpp version: " << Term::style(Term::Style::RESET) << Term::VERSION 
              << Term::style(Term::Style::BOLD) << "\nRepo: " << Term::style(Term::Style::RESET) << Term::REPO 
              << std::endl << std::endl;

    if (Term::stdout_connected()) 
        std::cout << Term::color_fg(Term::ColorBit4::GREEN) << "Standard output is attached to a terminal." << std::endl << std::endl;
    else
        std::cout << Term::color_fg(Term::ColorBit4::RED) << "Standard output is not attached to a terminal." << std::endl << std::endl;
    std::cout << Term::color_fg(Term::ColorBit4::DEFAULT);
    
    while (true) {
        Key pressed_key = Term::getkey();

        std::cout << "Key pressed: " << key_name(pressed_key) << std::endl;
        if (pressed_key == Key::CTRL_C)
            break; // Exit the loop
    }

    return 0;
}
