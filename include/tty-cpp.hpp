

// begin --- color.cpp --- 



// begin --- term.h --- 

#pragma once

#include <cstdint>
#include <string>
#include <exception>
#include <stdexcept>
#include <unistd.h>
#include <termios.h>

namespace Term {
const std::string Version = "1.0.0";
const std::string Repo = "https://github.com/ZackeryRSmith/tty-cpp";

/********************* NAMESPACE PRIVATE *********************/
namespace Private {
std::string getenv(const std::string&);

// returns the terminal size as (rows, columns) / (Y, X), throws a runtime error
// if the console is not connected
std::pair<std::size_t, std::size_t> get_term_size();

// Returns true if a character is read, otherwise immediately returns false
// This can't be made inline
bool read_raw(char* s);

char read_raw_stdin();
}
/********************* NAMESPACE PRIVATE *********************/

/********************** EXCEPTION CLASS **********************/
class Exception : public std::exception {
public:
  Exception(const std::string& what) : m_what(what){};
  virtual const char* what() const noexcept override { return m_what.c_str(); }
private:
  std::string m_what;
};
/********************** EXCEPTION CLASS **********************/

bool is_a_tty(const FILE*);
bool is_stdin_a_tty();
bool is_stdout_a_tty();
bool is_stderr_a_tty();

termios get_term_attributes();
void term_load(termios); // load terminal attributes from the passed termios struct
void keyboard_enabled(bool);
void disable_signal_keys(bool);

std::pair<std::size_t, std::size_t> get_size(); // get terminal size (column, row)
bool stdin_connected(); // check if stdin is connected to a TTY
bool stdout_connected(); // check if stdout is connect to a TTY
std::string cursor_off();
std::string cursor_on();
std::string clear_screen();
std::string clear_buffer(); // clear screen and the scroll-back buffer
// move the cursor to the given (row, column) / (Y, X)
std::string cursor_move(std::size_t row, std::size_t column); // move cursor to given column and row
std::string cursor_up(); // move the cursor up N lines (default N=1)
std::string cursor_up(std::size_t rows);
std::string cursor_down(); // move the cursor down N lines (default N=1)
std::string cursor_down(std::size_t rows);
std::string cursor_right(); // move the cursor forward N columns (default N=1)
std::string cursor_right(std::size_t columns);
std::string cursor_left(); // move the cursor backward N columns (default N=1)
std::string cursor_left(std::size_t columns);
std::pair<std::size_t, std::size_t> cursor_position(); // returns the current cursor position (row, column)
std::string cursor_position_report(); // the ANSI code to generate a cursor position report
std::string clear_eol(); // clear from cursor position to the end of the line
std::string screen_save(); // save screen state
std::string screen_load(); // restore last saved screen state
std::string terminal_title(const std::string& title); // change the terminal title (supported by only a few terminals)

} // namespace Term



// end --- term.h --- 



// begin --- color.h --- 

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



// end --- color.h --- 



#include <cerrno>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>


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

std::string Term::style(Term::Style style) { return "\033[" + std::to_string((std::uint8_t)style) + 'm'; }



// end --- color.cpp --- 



// begin --- input.cpp --- 

// Fix possible bug with GCC compilers
#if !defined(_GLIBCXX_USE_NANOSLEEP)
  #define _GLIBCXX_USE_NANOSLEEP
#endif

// begin --- input.h --- 

#pragma once
#include <cstdint>
#include <string>

namespace Term {
enum Key : std::int32_t {
    NO_KEY = -1,
    NUL,                
    CTRL_A,             
    CTRL_B,             
    CTRL_C,             
    CTRL_D,             
    CTRL_E,             
    CTRL_F,             
    CTRL_G,             
    BACKSPACE,          
    TAB,
    ENTER,              
    LF,                 
    CTRL_K,             
    CTRL_L,             
    CR, // mapped to ENTER
    CTRL_N,            
    CTRL_O,             
    CTRL_P,             
    CTRL_Q,             
    CTRL_R,             
    CTRL_S,             
    CTRL_T,             
    CTRL_U,             
    CTRL_V,             
    CTRL_W,             
    CTRL_X,             
    CTRL_Y,             
    CTRL_Z,             
    ESC,                
    CTRL_SLASH,         
    CTRL_CLOSE_BRACKET, 
    CTRL_CARET,         
    CTRL_UNDERSCORE,    
    SPACE,              
    EXCLAMATION_MARK,   
    QUOTE,              
    HASH,               
    DOLLAR,             
    PERCENT,            
    AMPERSAND,          
    APOSTROPHE,         
    OPEN_PARENTHESIS,   
    CLOSE_PARENTHESIS,  
    ASTERISK,           
    PLUS,               
    COMMA,              
    HYPHEN,             
    MINUS,              
    PERIOD,             
    SLASH,              
    ZERO,               
    ONE,                
    TWO,                
    THREE,              
    FOUR,               
    FIVE,               
    SIX,                
    SEVEN,              
    EIGHT,              
    NINE,               
    COLON,              
    SEMICOLON,          
    LESS_THAN,          
    OPEN_CHEVRON,       
    EQUAL,              
    GREATER_THAN,       
    CLOSE_CHEVRON,      
    QUESTION_MARK,      
    AROBASE,            
    A,                  
    B,                  
    C,                  
    D,                  
    E,                  
    F,                  
    G,                  
    H,                  
    I,                  
    J,                  
    K,                  
    L,                  
    M,                  
    N,                  
    O,                  
    P,                  
    Q,                  
    R,                  
    S,                  
    T,                  
    U,                  
    V,                  
    W,                  
    X,                  
    Y,                  
    Z,                  
    OPEN_BRACKET,       
    BACKSLASH,          
    CLOSE_BRACKET,      
    CARET,              
    UNDERSCORE,         
    GRAVE_ACCENT,       
    a,                  
    b,                  
    c,                  
    d,                  
    e,                  
    f,                  
    g,                  
    h,                  
    i,                  
    j,                  
    k,                  
    l,                  
    m,                  
    n,                  
    o,                  
    p,                  
    q,                  
    r,                  
    s,                  
    t,                  
    u,                  
    v,                  
    w,                  
    x,                  
    y,                  
    z,                  
    OPEN_BRACE,         
    VERTICAL_BAR,       
    CLOSE_BRACE,        
    TILDE,              
    DEL,        
  
    // extended ANSII
    ALT_ENTER = 256,
    ARROW_LEFT,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    CTRL_UP,
    CTRL_DOWN,
    CTRL_RIGHT,
    CTRL_LEFT,
    NUMERIC_5,
    HOME,
    INSERT,
    END,
    PAGE_UP,
    PAGE_DOWN,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
  
    // modifier keys
    CTRL = -AROBASE,
    ALT = (1 << 9)
};

bool is_ASCII(const Key&); // detect if Key is convertible to ANSII 
bool is_extended_ASCII(const Key&); // detect if Key is convertible to Extended ANSII
bool is_ALT(const Key&); // detect if Key is being effected by the ALT modifier
bool is_CTRL(const Key&); // detect if Key is being effected by the CTRL modifier

// waits for a key press, translates escape codes
// if Term:Terminal is not enabling the keyboard it'll loop for infinity
std::int32_t read_key();

// If there was a key press, returns the translated key from escape codes,
// otherwise returns 0. If the escape code is not supported it returns a
// negative number.
// if Term::Terminal is not enabling the keyboard it'll always return 0
std::int32_t read_key0();

// returns the stdin as a string
// waits until the EOT signal is sent
// if Term::Terminal is not enabling the keyboard this function will wait until
// the user presses CTRL+D (which sends the EOT signal)
std::string read_stdin();
} // namespace Term



// end --- input.h --- 



#include <chrono>
#include <thread>
#include <type_traits>

bool Term::is_ASCII(const Term::Key& key) { return key >= 0 && key <= 127; }
bool Term::is_extended_ASCII(const Term::Key& key) { return key >= 0 && key <= 255; }

bool Term::is_ALT(const Term::Key& key) { return (key & Key::ALT) == Key::ALT; }
bool Term::is_CTRL(const Term::Key& key) {
    // Need to suppress the TAB etc...
    return key > 0 && key <= 31 && key != Key::BACKSPACE && key != Key::TAB && key != ESC && key != Key::LF && key != CR;
}


std::int32_t Term::read_key() {
    std::int32_t key{Key::NO_KEY};

    while((key = read_key0()) == Key::NO_KEY)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return key;
}

std::int32_t Term::read_key0() {
    char c{'\0'};
    if(!Private::read_raw(&c))
        return Key::NO_KEY;
    if(is_CTRL(static_cast<Term::Key>(c)))
        return c;
    else if(c == Key::ESC) {
        char seq[4]{'\0', '\0', '\0', '\0'};
  
        if(!Private::read_raw(&seq[0])) return Key::ESC;
        if(!Private::read_raw(&seq[1])) {
            if(seq[0] >= 'a' && seq[0] <= 'z')
                return Key::ALT + seq[0];
            if(seq[0] == '\x0d')
                return Key::ALT_ENTER;
            return -1;
        }

        if(seq[0] == '[') {
            if(seq[1] >= '0' && seq[1] <= '9') {
                if(!Private::read_raw(&seq[2]))
                    return -2;
                if(seq[2] == '~') {
                    switch(seq[1]) {
                    case '1': return Key::HOME;
                    case '2': return Key::INSERT;
                    case '3': return Key::DEL;
                    case '4': return Key::END;
                    case '5': return Key::PAGE_UP;
                    case '6': return Key::PAGE_DOWN;
                    case '7': return Key::HOME;
                    case '8': return Key::END;
                    }
                }
            
                else if(seq[2] == ';') {
                    if(seq[1] == '1') {
                        if(!Private::read_raw(&seq[2]))
                            return -10;
                        if(!Private::read_raw(&seq[3]))
                            return -11;
                        if(seq[2] == '5') {
                            switch(seq[3]) {
                            case 'A': return Key::CTRL_UP;
                            case 'B': return Key::CTRL_DOWN;
                            case 'C': return Key::CTRL_RIGHT;
                            case 'D': return Key::CTRL_LEFT;
                            }
                        }
                        return -12;
                    }
                }
                
                else {
                    if(seq[2] >= '0' && seq[2] <= '9') {
                        if(!Private::read_raw(&seq[3]))
                            return -3;
                        if(seq[3] == '~') {
                            if(seq[1] == '1') {
                                switch(seq[2]) {
                                case '5': return Key::F5;
                                case '7': return Key::F6;
                                case '8': return Key::F7;
                                case '9': return Key::F8;
                                }
                            }
                            else if(seq[1] == '2') {
                                switch(seq[2]) {
                                case '0': return Key::F9;
                                case '1': return Key::F10;
                                case '3': return Key::F11;
                                case '4': return Key::F12;
                                }
                            }
                        }
                    }
                }
            }
            else {
                switch(seq[1]) {
                case 'A': return Key::ARROW_UP;
                case 'B': return Key::ARROW_DOWN;
                case 'C': return Key::ARROW_RIGHT;
                case 'D': return Key::ARROW_LEFT;
                case 'E': return Key::NUMERIC_5;
                case 'H': return Key::HOME;
                case 'F': return Key::END;
                }
            }
        }
        else if(seq[0] == 'O') {
            switch(seq[1]) {
            case 'F': return Key::END;
            case 'H': return Key::HOME;
            case 'P': return Key::F1;
            case 'Q': return Key::F2;
            case 'R': return Key::F3;
            case 'S': return Key::F4;
            }
        }
        return -4;
    }
    else {
        switch(c) {
        case Key::DEL: return Key::BACKSPACE;
        case Key::LF:
        case Key::CR: return Key::ENTER;
        }
        if(c == '\xc3') {
            if(!Private::read_raw(&c)) 
                return -8;
            else {
                if(c >= '\xa1' && c <= '\xba')
                    return Key::ALT + (c + 'a' - '\xa1');
                return -9;
            }
        }
        else if(c == '\xc2') {
            if(!Private::read_raw(&c))
                return -10;
            else {
                if(c == '\x8d')
                    return Key::ALT_ENTER;
                return -11;
            }
        }
        return c;
    }
}

// returns the whole input from STDIN as string
std::string Term::read_stdin() {
    std::string file;
    char c{'\0'};
    while(true) {
        c = Private::read_raw_stdin();
        if(c == 0x04) 
            return file;
        else
            file.push_back(c);
    }
}


// end --- input.cpp --- 



// begin --- term.cpp --- 



#include <cerrno>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <termios.h>

/********************* NAMESPACE PRIVATE *********************/
std::string Term::Private::getenv(const std::string& env) {
    if (std::getenv(env.c_str()) != nullptr) 
        return static_cast<std::string>(std::getenv(env.c_str()));
    else 
        return std::string();
}

std::pair<std::size_t, std::size_t> Term::Private::get_term_size() {
    struct winsize ws {};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
        throw Term::Exception("Couldn't get terminal size. Are we even connected to a TTY?");
    else
        return std::pair<std::size_t, std::size_t>{ws.ws_col, ws.ws_row};
}

char Term::Private::read_raw_stdin() {
    char c = getchar();
    if (c >= 0) 
        return c;
    else if (c == EOF)
        // In non-raw (blocking) mode this happens when the input file
        // ends. In such a case, return the End of Transmission (EOT)
        // character (Ctrl-D)
        return 0x04;
    else
        throw Term::Exception("getchar() failed");
}

bool Term::Private::read_raw(char* s) {
  // do nothing when TTY is not connected
  if (!is_stdin_a_tty())
      return false;
  
  ::ssize_t nread = ::read(0, s, 1);
  if (nread == -1 && errno != EAGAIN) 
    throw Term::Exception("read() failed");
  return (nread == 1);
}
/*************************************************************/


bool Term::is_a_tty(const FILE* fd) { return isatty(fileno(const_cast<FILE*>(fd))); }
bool Term::is_stdin_a_tty() { return is_a_tty(stdin); }
bool Term::is_stdout_a_tty() { return is_a_tty(stdout); }
bool Term::is_stderr_a_tty() { return is_a_tty(stderr); }

termios Term::get_term_attributes() {
    struct termios term;
    if (tcgetattr(STDIN_FILENO, &term) != 0)
        throw Term::Exception("tcgetattr() failed");
    return term;
}
void Term::term_load(termios term) { tcsetattr(STDIN_FILENO, TCSANOW, &term); }
void Term::keyboard_enabled(bool keyboard_enabled) {
    termios raw{};
    if (keyboard_enabled) {
        if(tcgetattr(0, &raw) == -1) { throw Term::Exception("tcgetattr() failed"); }
        
        raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        raw.c_cflag |= CS8;
        raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);
        raw.c_cc[VMIN]  = 0;
        raw.c_cc[VTIME] = 0;
        if(tcsetattr(0, TCSAFLUSH, &raw) == -1) { throw Term::Exception("tcsetattr() failed"); }
    }
}
void Term::disable_signal_keys(bool signal_keys_disabled) {
    termios raw{};
    if (signal_keys_disabled) {
        if(tcgetattr(0, &raw) == -1) { throw Term::Exception("tcgetattr() failed"); }
        raw.c_lflag &= ~ISIG;
        if(tcsetattr(0, TCSAFLUSH, &raw) == -1) { throw Term::Exception("tcsetattr() failed"); }
    }
}

std::pair<std::size_t, std::size_t> Term::get_size() { return Private::get_term_size(); }
bool Term::stdin_connected() { return Term::is_stdin_a_tty(); }
bool Term::stdout_connected() { return Term::is_stdout_a_tty(); }
std::string Term::cursor_off() { return "\x1b[?25l"; }
std::string Term::cursor_on() { return "\x1b[?25h"; }
std::string Term::clear_screen() { return "\033[2J"; }
std::string Term::clear_buffer() { return "\033[3J"; }
std::string Term::cursor_move(std::size_t row, std::size_t column) { return "\033[" + std::to_string(row) + ';' + std::to_string(column) + 'H'; }
std::string Term::cursor_up() { return "\033[1A"; }
std::string Term::cursor_up(std::size_t rows) { return "\033[" + std::to_string(rows) + 'A'; }
std::string Term::cursor_down() { return "\033[1B"; }
std::string Term::cursor_down(std::size_t rows) { return "\033[" + std::to_string(rows) + 'B'; }
std::string Term::cursor_right() { return "\033[1C"; }
std::string Term::cursor_right(std::size_t columns) { return "\033[" + std::to_string(columns) + 'C'; }
std::string Term::cursor_left() { return "\033[1D"; }
std::string Term::cursor_left(std::size_t columns) { return "\033[" + std::to_string(columns) + 'D'; }
std::pair<std::size_t, std::size_t> Term::cursor_position() {
    // write cursor position report
    std::cout << cursor_position_report() << std::flush;
    // read input buffer
    std::string buf;
    char c{'\0'};
    do {
        while(!Private::read_raw(&c));
        buf.push_back(c);
    } while(c != 'R');

    bool found{false};
    std::size_t row{0};
    std::size_t column{0};
    for(std::size_t i = 2; i < buf.size(); i++) {
        if (buf[i] == ';') 
            found = true;
        else if (found == false && buf[i] >= '0' && buf[i] <= '9')
            row = row * 10 + (buf[i] - '0');
        else if (found == true && buf[i] >= '0' && buf[i] <= '9')
            column = column * 10 + (buf[i] - '0');
    }
    return std::pair<std::size_t, std::size_t>(row, column);
}

std::string Term::cursor_position_report() { return "\x1b[6n"; }
std::string Term::clear_eol() { return "\033[K"; }
std::string Term::screen_save() { return "\0337\033[?1049h"; }
std::string Term::screen_load() { return "\033[?1049l\0338"; }
std::string Term::terminal_title(const std::string& title) { return "\033]0;" + title + '\a'; }


// end --- term.cpp --- 

