#include <iostream>
#include <string>
#include <variant>
#include "../include/tty-cpp.hpp"

/**
 * @brief Fills the screen with a given character and color.
 *
 * @param c The character to fill the screen with.
 * @param color The color to use for filling the screen. Default value is no color.
 *
 * @details This function fills the entire screen with the specified character and color (if provided).
 *
 * Usage:
 *  - To fill the screen with a character and color:
 *      - fill_screen('x', Term::rgb(255, 0, 0)); // fills the screen with red 'x'
 *      - fill_screen('-', Term::ColorBit4::Blue); // fills the screen with blue '-'
 *  - To fill the screen with a character and no color:
 *      - fill_screen('*'); // fills the screen with '*' in the default background color
 *
 */
void fill_screen(char c, std::variant<std::monostate, Term::rgb, Term::ColorBit4> color = std::variant<std::monostate, Term::rgb, Term::ColorBit4>{}) {
    Screen::clear();
    Screen::Size term_size = Screen::size();
    
    std::string fill_color = "";

    if (!std::holds_alternative<std::monostate>(color)) {
        if (std::holds_alternative<Term::rgb>(color)) {
            fill_color = Term::color_bg(std::get<Term::rgb>(color));
        } else {
            fill_color = Term::color_bg(std::get<Term::ColorBit4>(color));
        }
    }

    for (int y = 0; y < term_size.columns; y++) {
        for (int x = 0; x < term_size.rows; x++) {
            std::cout << fill_color << c;
        }
    }

    Cursor::home();
}

/**
 * @brief Fills the screen with a given character and RGB color.
 *
 * @param c The character to fill the screen with.
 * @param r The red component of the RGB color.
 * @param g The green component of the RGB color.
 * @param b The blue component of the RGB color.
 *
 * @details This function fills the entire screen with the specified character and RGB color.
 *
 * Usage:
 *  - To fill the screen with a character and RGB color:
 *      - fill_screen('x', 255, 0, 0); // fills the screen with red 'x'
 *      - fill_screen('-', 0, 0, 255); // fills the screen with blue '-'
 *
 */
void fill_screen(char c, size_t r, size_t g, size_t b) { 
    fill_screen(c, Term::rgb(r, g, b)); 
}

int main() {
    RawModeGuard raw_mode_guard;
    Cursor::hide();

    fill_screen(' ');

    for(;;) if (Term::keyhit()) break;

    Cursor::show();

    return 0;
}
