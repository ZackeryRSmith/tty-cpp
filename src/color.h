#pragma once

#include <cstdint>
#include <string>
#include <exception>
#include <stdexcept>
#include <unistd.h>

namespace Term {
enum class ColorBit4 : std::uint8_t {
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    
    DEFAULT        = 9,
    GRAY           = 60,
    RED_BRIGHT     = 61,
    GREEN_BRIGHT   = 62,
    YELLOW_BRIGHT  = 63,
    BLUE_BRIGHT    = 64,
    MAGENTA_BRIGHT = 65,
    CYAN_BRIGHT    = 66,
    WHITE_BRIGHT   = 67
};

enum Style : std::uint8_t {
    RESET,
    BOLD,
    DIM,
    ITALIC,
    UNDERLINE,
    BLINK,
    BLINK_RAPID,
    CONCEL,
    CROSSED, // mostly supported
    OVERLINE = 53 // barely supported 
};

// class for representing 24bit color
class rgb {
public:
    rgb() = default;
    rgb(const std::uint8_t& _r, const std::uint8_t& _g, const std::uint8_t& _b) : r(_r), g(_g), b(_b), empty(false) {}
    std::uint8_t r{0};
    std::uint8_t g{0};
    std::uint8_t b{0};
    bool empty{true};
};

// reference colors for converting 24bit colors to 4bit colors (and vice versa)
class Bit4Reference {
public:
  static rgb BLACK;
  static rgb RED;
  static rgb GREEN;
  static rgb YELLOW;
  static rgb BLUE;
  static rgb MAGENTA;
  static rgb CYAN;
  static rgb WHITE;
  static rgb GRAY;
  static rgb RED_BRIGHT;
  static rgb GREEN_BRIGHT;
  static rgb YELLOW_BRIGHT;
  static rgb BLUE_BRIGHT;
  static rgb MAGENTA_BRIGHT;
  static rgb CYAN_BRIGHT;
  static rgb WHITE_BRIGHT;
  static rgb NONE;
};

rgb bit4_to_rgb(ColorBit4);
rgb bit8_to_rgb(std::uint8_t);
rgb bit24_to_rgb(std::uint8_t, std::uint8_t, std::uint8_t);
rgb rgb_empty();

// compares two 24bit colors and returns the differnce between them
std::uint16_t rgb_compare(rgb, rgb);

ColorBit4 rgb_to_bit4(rgb);
uint8_t rgb_to_bit8(rgb);

bool bit24_support();
// returns ANSI code for 24bit colors (if not supported these functions will fall back to 8bit)
std::string rgb_to_bit24_auto_fg(rgb color); 
std::string rgb_to_bit24_auto_bg(rgb color);

std::string color_fg(ColorBit4); // set 4bit color
std::string color_fg(std::uint8_t); // set 8bit color
std::string color_fg(std::uint8_t, std::uint8_t, std::uint8_t); // set 24bit color
std::string color_fg(rgb); // alternative way to set 24 bit color
//std::string color_fg(rgbf); // set foreground color present in the passed RGBF struct

std::string color_bg(ColorBit4 color); // set 4bit color
std::string color_bg(std::uint8_t color); // set 8bit color
// Set the given 24bit color
std::string color_bg(std::uint8_t r, std::uint8_t g, std::uint8_t b); // set 24bit color
std::string color_bg(rgb rgb); // alternative way to set 24 bit color
//std::string color_bg(rgbf rgbf); // set background color present in the passed RGBF struct

std::string style(Style style); // set style

} // namespace Term

