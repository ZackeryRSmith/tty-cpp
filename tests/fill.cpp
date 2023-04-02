#include <iostream>
#include <string>
#include "../include/tty-cpp.hpp"

void fill_screen(char c, Term::rgb color) {
    Screen::clear();
    Screen::Size term_size = Screen::size();
    
    std::string fill_color = Term::color_bg(color);

    for (int y = 0; y < term_size.columns; y++) {
        for (int x = 0; x < term_size.rows; x++) {
            std::cout << fill_color << c;
        }
    }

    Cursor::home();
}

int main() {
    RawModeGuard raw_mode_guard;
    Cursor::hide();
    fill_screen(' ', Term::bit4_to_rgb(Term::ColorBit4::BLUE_BRIGHT));

    Screen::Size term_size = Screen::size();

    Key key;
    while ((key = Term::getkey()) != Key::CTRL_C) {

    }

    Cursor::show();

    return 0;
}
