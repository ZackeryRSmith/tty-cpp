#include "headers/term.h"
#include "headers/color.h"

#include <cerrno>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>


/****************** GENERAL COLOR FUNCTIONS ******************/
bool Term::bit24_support() {
  std::string colorterm = Term::Private::getenv("COLORTERM");
  if (colorterm == "truecolor" || colorterm == "24bit") { return true; }
  return false;
}

Term::rgb Term::rgb_empty() { return rgb{}; }

uint16_t Term::rgb_compare(rgb color_first, rgb color_second) {
    std::uint16_t diff = 0;
    // red
    if (color_first.r > color_second.r)
        diff += color_first.r - color_second.r;
    else 
        diff += color_second.r - color_first.r;
    
    // green
    if (color_first.g > color_second.g)
        diff += color_first.g - color_second.g;
    else
        diff += color_second.g - color_first.g;
    
    // blue
    if (color_first.b > color_second.b)
        diff += color_first.b - color_second.b;
    else
        diff += color_second.b - color_first.b;

    return diff;
}
/*************************************************************/


/********************* COLOR CONVERSIONS *********************/
Term::rgb Term::Bit4Reference::BLACK(0, 0, 0);
Term::rgb Term::Bit4Reference::RED(151, 12, 40);
Term::rgb Term::Bit4Reference::GREEN(1, 142, 66);
Term::rgb Term::Bit4Reference::YELLOW(238, 198, 67);
Term::rgb Term::Bit4Reference::BLUE(13, 33, 161);
Term::rgb Term::Bit4Reference::MAGENTA(255, 0, 144);
Term::rgb Term::Bit4Reference::CYAN(0, 159, 184);
Term::rgb Term::Bit4Reference::WHITE(240, 240, 240);
Term::rgb Term::Bit4Reference::GRAY(127, 127, 127);
Term::rgb Term::Bit4Reference::RED_BRIGHT(241, 85, 116);
Term::rgb Term::Bit4Reference::GREEN_BRIGHT(52, 254, 146);
Term::rgb Term::Bit4Reference::YELLOW_BRIGHT(243, 215, 124);
Term::rgb Term::Bit4Reference::BLUE_BRIGHT(63, 136, 197);
Term::rgb Term::Bit4Reference::MAGENTA_BRIGHT(255, 92, 184);
Term::rgb Term::Bit4Reference::CYAN_BRIGHT(51, 228, 255);
Term::rgb Term::Bit4Reference::WHITE_BRIGHT(255, 255, 255);
Term::rgb Term::Bit4Reference::NONE{};

Term::rgb Term::bit4_to_rgb(const Term::ColorBit4 color) {
    switch (color) {
    case ColorBit4::BLACK:          return Bit4Reference::BLACK;
    case ColorBit4::RED:            return Bit4Reference::RED;
    case ColorBit4::GREEN:          return Bit4Reference::GREEN;
    case ColorBit4::YELLOW:         return Bit4Reference::YELLOW;
    case ColorBit4::BLUE:           return Bit4Reference::BLUE;
    case ColorBit4::MAGENTA:        return Bit4Reference::MAGENTA;
    case ColorBit4::CYAN:           return Bit4Reference::CYAN;
    case ColorBit4::WHITE:          return Bit4Reference::WHITE;
    case ColorBit4::DEFAULT:        return Bit4Reference::NONE;
    case ColorBit4::GRAY:           return Bit4Reference::GRAY;
    case ColorBit4::RED_BRIGHT:     return Bit4Reference::RED_BRIGHT;
    case ColorBit4::GREEN_BRIGHT:   return Bit4Reference::GREEN_BRIGHT;
    case ColorBit4::YELLOW_BRIGHT:  return Bit4Reference::YELLOW_BRIGHT;
    case ColorBit4::BLUE_BRIGHT:    return Bit4Reference::BLUE_BRIGHT;
    case ColorBit4::MAGENTA_BRIGHT: return Bit4Reference::MAGENTA_BRIGHT;
    case ColorBit4::CYAN_BRIGHT:    return Bit4Reference::CYAN_BRIGHT;
    case ColorBit4::WHITE_BRIGHT:   return Bit4Reference::WHITE_BRIGHT;
    }

    // impossible case
    return {};
}

Term::rgb Term::bit24_to_rgb(std::uint8_t r, std::uint8_t g, std::uint8_t b) { return rgb(r, g, b); }
Term::ColorBit4 Term::rgb_to_bit4(Term::rgb color) {
    if (color.empty)
        return ColorBit4::DEFAULT;
  
    // set initial start value
    ColorBit4 color_result = ColorBit4::BLACK;
    std::uint16_t diff = rgb_compare(color, Term::Bit4Reference::BLACK);
  
    // compare all colors
    if (diff > rgb_compare(color, Bit4Reference::RED)) {
        diff = rgb_compare(color, Bit4Reference::RED);
        color_result = ColorBit4::RED;
    }
    if (diff > rgb_compare(color, Bit4Reference::GREEN)) {
        diff = rgb_compare(color, Bit4Reference::GREEN);
        color_result = ColorBit4::GREEN;
    }
    if (diff > rgb_compare(color, Bit4Reference::YELLOW)) {
        diff = rgb_compare(color, Bit4Reference::YELLOW);
        color_result = ColorBit4::YELLOW;
    }
    if (diff > rgb_compare(color, Bit4Reference::BLUE)) {
        diff = rgb_compare(color, Bit4Reference::BLUE);
        color_result = ColorBit4::BLUE;
    }
    if (diff > rgb_compare(color, Bit4Reference::MAGENTA)) {
        diff = rgb_compare(color, Bit4Reference::MAGENTA);
        color_result = ColorBit4::MAGENTA;
    }
    if (diff > rgb_compare(color, Bit4Reference::CYAN)) {
        diff = rgb_compare(color, Bit4Reference::CYAN);
        color_result = ColorBit4::CYAN;
    }
    if (diff > rgb_compare(color, Bit4Reference::WHITE)) {
        diff = rgb_compare(color, Bit4Reference::WHITE);
        color_result = ColorBit4::WHITE;
    }
    if (diff > rgb_compare(color, Bit4Reference::GRAY)) {
        diff = rgb_compare(color, Bit4Reference::GRAY);
        color_result = ColorBit4::GRAY;
    }
    if (diff > rgb_compare(color, Bit4Reference::RED_BRIGHT)) {
        diff = rgb_compare(color, Bit4Reference::RED_BRIGHT);
        color_result = ColorBit4::RED_BRIGHT;
    }
    if (diff > rgb_compare(color, Bit4Reference::GREEN_BRIGHT)) {
        diff = rgb_compare(color, Bit4Reference::GREEN_BRIGHT);
        color_result = ColorBit4::GREEN_BRIGHT;
    }
    if (diff > rgb_compare(color, Bit4Reference::YELLOW_BRIGHT)) {
        diff = rgb_compare(color, Bit4Reference::YELLOW_BRIGHT);
        color_result = ColorBit4::YELLOW_BRIGHT;
    }
    if (diff > rgb_compare(color, Bit4Reference::BLUE_BRIGHT)) {
        diff = rgb_compare(color, Bit4Reference::BLUE_BRIGHT);
        color_result = ColorBit4::BLUE_BRIGHT;
    }
    if (diff > rgb_compare(color, Bit4Reference::MAGENTA_BRIGHT)) {
        diff = rgb_compare(color, Bit4Reference::MAGENTA_BRIGHT);
        color_result = ColorBit4::MAGENTA_BRIGHT;
    }
    if (diff > rgb_compare(color, Bit4Reference::CYAN_BRIGHT)) {
        diff = rgb_compare(color, Bit4Reference::CYAN_BRIGHT);
        color_result = ColorBit4::CYAN_BRIGHT;
    }
    if (diff > rgb_compare(color, Bit4Reference::WHITE_BRIGHT))
        color_result = ColorBit4::WHITE_BRIGHT;
    return color_result;
}
Term::ColorBit4 Term::rgb_to_bit4(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
    return Term::rgb_to_bit4(rgb(r, g, b));
}

std::uint8_t Term::rgb_to_bit8(rgb color) {
    if (color.empty) return 0;
    if      (rgb_compare(color, Bit4Reference::BLACK         ) < 1) return 0;
    else if (rgb_compare(color, Bit4Reference::RED           ) < 1) return 1;
    else if (rgb_compare(color, Bit4Reference::GREEN         ) < 1) return 2;
    else if (rgb_compare(color, Bit4Reference::YELLOW        ) < 1) return 3;
    else if (rgb_compare(color, Bit4Reference::BLUE          ) < 1) return 4;
    else if (rgb_compare(color, Bit4Reference::MAGENTA       ) < 1) return 5;
    else if (rgb_compare(color, Bit4Reference::CYAN          ) < 1) return 6;
    else if (rgb_compare(color, Bit4Reference::WHITE         ) < 1) return 7;
    else if (rgb_compare(color, Bit4Reference::GRAY          ) < 1) return 8;
    else if (rgb_compare(color, Bit4Reference::RED_BRIGHT    ) < 1) return 9;
    else if (rgb_compare(color, Bit4Reference::GREEN_BRIGHT  ) < 1) return 10;
    else if (rgb_compare(color, Bit4Reference::YELLOW_BRIGHT ) < 1) return 11;
    else if (rgb_compare(color, Bit4Reference::BLUE_BRIGHT   ) < 1) return 12;
    else if (rgb_compare(color, Bit4Reference::MAGENTA_BRIGHT) < 1) return 13;
    else if (rgb_compare(color, Bit4Reference::CYAN_BRIGHT   ) < 1) return 14;
    else if (rgb_compare(color, Bit4Reference::WHITE_BRIGHT  ) < 1) return 15;

    // check gray scale in 24 steps
    if (color.r == color.g && color.r == color.b && color.g == color.b) { return 232 + color.r / 32 + color.g / 32 + color.b / 32; }

    // normal color space
    return 16 + 36 * (color.r / 51) + 6 * (color.g / 51) + (color.b / 51);
}
std::uint8_t Term::rgb_to_bit8(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
    return Term::rgb_to_bit8(rgb(r, g, b));
}
/*************************************************************/


std::string Term::rgb_to_bit24_auto_fg(rgb color) { // :MAYBE-USELESS:
    if (bit24_support())
        return color_fg(color);
    else
        return color_fg(rgb_to_bit8(color));
}

std::string Term::rgb_to_bit24_auto_bg(rgb color) { // :MAYBE-USELESS:
    if (bit24_support()) 
        return color_bg(color);
    else
        return color_bg(rgb_to_bit8(color));
}

/* FOREGROUND COLORS */
std::string Term::color_fg(Term::ColorBit4 color) { return "\033[" + std::to_string((std::uint8_t)color + 30) + 'm'; }
std::string Term::color_fg(std::uint8_t color) { return "\033[38;5;" + std::to_string(color) + 'm'; }
std::string Term::color_fg(std::uint8_t r, std::uint8_t g, std::uint8_t b) { return "\033[38;2;" + std::to_string(r) + ';' + std::to_string(g) + ';' + std::to_string(b) + 'm'; }
std::string Term::color_fg(Term::rgb rgb) {
    if (rgb.empty)
        return color_fg(ColorBit4::DEFAULT);  // resets the current terminal color
    return "\033[38;2;" + std::to_string(rgb.r) + ';' + std::to_string(rgb.g) + ';' + std::to_string(rgb.b) + 'm';
}

/* BACKGROUND COLORS */
std::string Term::color_bg(Term::ColorBit4 color) { return "\033[" + std::to_string((uint8_t)color + 40) + 'm'; }
std::string Term::color_bg(std::uint8_t color) { return "\033[48;5;" + std::to_string(color) + 'm'; }
std::string Term::color_bg(std::uint8_t r, std::uint8_t g, std::uint8_t b) { return "\033[48;2;" + std::to_string(r) + ';' + std::to_string(g) + ';' + std::to_string(b) + 'm'; }
std::string Term::color_bg(Term::rgb rgb) {
    if (rgb.empty)
        return color_bg(ColorBit4::DEFAULT);  // resets the current terminal color
    return "\033[48;2;" + std::to_string(rgb.r) + ';' + std::to_string(rgb.g) + ';' + std::to_string(rgb.b) + 'm';
}

std::string Term::style(Term::Style style) {
    switch (style) { /* improve me, concatenation doesn't work for some reason */
        case Style::RESET:         return "\033[0m";
        case Style::BOLD:          return "\033[1m";
        case Style::DIM:           return "\033[2m";
        case Style::ITALIC:        return "\033[3m";
        case Style::UNDERLINE:     return "\033[4m";
        case Style::BLINK:         return "\033[5m";
        case Style::REVERSE:       return "\033[7m";
        case Style::CONCEAL:       return "\033[8m";
        case Style::STRIKETHROUGH: return "\033[9m";
        case Style::OVERLINE:      return "\033[53m";
        default:                   return "";
    }
}

