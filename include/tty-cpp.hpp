

// begin --- headers --- 



// end --- headers --- 



// begin --- helpers.cpp --- 



// begin --- term.h --- 

#pragma once

#include <cstdint>
#include <string>
#include <exception>
#include <stdexcept>
#include <unistd.h>
#include <termios.h>
#include <iostream>

namespace Term {
const std::string VERSION = "1.5.0";
const std::string REPO = "https://github.com/ZackeryRSmith/tty-cpp";


/********************* NAMESPACE PRIVATE *********************/
namespace Private {
  std::string getenv(const std::string&);
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

inline void enable_raw_mode();
inline void disable_raw_mode();
inline bool is_raw_mode();

bool stdin_connected();  // check if stdin is connected to a TTY
bool stdout_connected(); // check if stdout is connect to a TTY
void enter_alt_buffer();                       // enter the alternate terminal buffer
void exit_alt_buffer();                        // exit the alternate terminal buffer
void terminal_title(const std::string& title); // change the terminal title (supported by only a few terminals)

} // namespace Term



// end --- term.h --- 



// begin --- helpers.h --- 

#pragma once

/* guard to make sure raw mode is exited on program exit */
class RawModeGuard {
public:
    RawModeGuard() { Term::enable_raw_mode(); }
    ~RawModeGuard() { Term::disable_raw_mode(); }
};

/* scope guard which executes a function upon deconstruction */
class ScopeGuard {
public:
    ScopeGuard(std::function<void()> f) : func(f) {}
    ~ScopeGuard() { func(); }

private:
    std::function<void()> func;
};




// end --- helpers.h --- 



// kind of not needed for right now :)


// end --- helpers.cpp --- 



// begin --- input.cpp --- 



// begin --- input.h --- 

#pragma once

enum class Key {
    UNKNOWN = -1,
    // ASCII values for control characters
    NUL = 0,
    SOH = 1,
    STX = 2,
    ETX = 3,
    EOT = 4,
    ENQ = 5,
    ACK = 6,
    BEL = 7,
    BS = 8,
    HT = 9,
    LF = 10,
    VT = 11,
    FF = 12,
    CR = 13,
    SO = 14,
    SI = 15,
    DLE = 16,
    DC1 = 17,
    DC2 = 18,
    DC3 = 19,
    DC4 = 20,
    NAK = 21,
    SYN = 22,
    ETB = 23,
    CAN = 24,
    EM = 25,
    SUB = 26,
    ESC = 27,
    FS = 28,
    GS = 29,
    RS = 30,
    US = 31,
    DEL = 32,
    // Regular characters (ASCII values 32 to 126)
    SPACE = 32,
    EXCLAMATION_MARK = 33,
    // ...
    TILDE = 126,
    // CTRL keys combinations
    CTRL_A = 128,
    CTRL_B,
    CTRL_C,
    CTRL_D,
    CTRL_E,
    CTRL_F,
    CTRL_G,
    CTRL_H,
    CTRL_I,
    CTRL_J,
    CTRL_K,
    CTRL_L,
    CTRL_M,
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
    // Add ASCII characters directly
    A = 'A',
    B = 'B',
    C = 'C',
    D = 'D',
    E = 'E',
    F = 'F',
    G = 'G',
    H = 'H',
    I = 'I',
    J = 'J',
    K = 'K',
    L = 'L',
    M = 'M',
    N = 'N',
    O = 'O',
    P = 'P',
    Q = 'Q',
    R = 'R',
    S = 'S',
    T = 'T',
    U = 'U',
    V = 'V',
    W = 'W',
    X = 'X',
    Y = 'Y',
    Z = 'Z',
    a = 'a',
    b = 'b',
    c = 'c',
    d = 'd',
    e = 'e',
    f = 'f',
    g = 'g',
    h = 'h',
    i = 'i',
    j = 'j',
    k = 'k',
    l = 'l',
    m = 'm',
    n = 'n',
    o = 'o',
    p = 'p',
    q = 'q',
    r = 'r',
    s = 's',
    t = 't',
    u = 'u',
    v = 'v',
    w = 'w',
    x = 'x',
    y = 'y',
    z = 'z',
    // Special keys
    ENTER = 1000,
    UP_ARROW,
    DOWN_ARROW,
    LEFT_ARROW,
    RIGHT_ARROW,
    CTRL_BASE,
    // Modifier keys
    ALT,
    SHIFT,
    // Add more keys as needed
};

namespace Term {
Key getkey();
int keyhit();
} // namespace Term


// end --- input.h --- 


#include <fcntl.h>
#include <sys/ioctl.h>

Key Term::getkey() {
    char c;

    if (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == '\x1b') {
            char seq[4];

            if (read(STDIN_FILENO, &seq[0], 1) != 1) return Key::ESC;
            if (read(STDIN_FILENO, &seq[1], 1) != 1) return Key::ESC;

            if (seq[0] == '[') {
                if (seq[1] >= '0' && seq[1] <= '9') {
                    if (read(STDIN_FILENO, &seq[2], 1) != 1) return Key::ESC;
                    if (seq[2] == '~') {
                        if (seq[1] == '3') return Key::DEL;
                    }
                } else {
                    switch (seq[1]) {
                        case 'A': return Key::UP_ARROW;
                        case 'B': return Key::DOWN_ARROW;
                        case 'C': return Key::RIGHT_ARROW;
                        case 'D': return Key::LEFT_ARROW;
                    }
                }
            }
        } else if (c == '\r') {
            return Key::ENTER;
        } else if (c == '\x7f') {
            return Key::BS;
        } else if (c >= 1 && c <= 26) {
            return static_cast<Key>(static_cast<int>(Key::CTRL_A) + (c - 1));
        } else {
            return static_cast<Key>(c);
        }
    }

    return Key::UNKNOWN;
}

int Term::keyhit() {
    int bytesWaiting;
    ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);

    return bytesWaiting;
}


// end --- input.cpp --- 



// begin --- screen.cpp --- 



// begin --- cursor.h --- 

#pragma once

struct cursor_pos_t {
    std::size_t row;
    std::size_t column;
};

namespace Cursor {   
inline void set_row(const std::size_t&);       // move to row
inline void set_column(const std::size_t&);    // move to column
inline void up(const std::size_t& = 1);        // move up N lines (default N=1)
inline void down(const std::size_t& = 1);      // move down N lines (default N=1)
inline void right(const std::size_t& = 1);     // move right N columns (default N=1)
inline void left(const std::size_t& = 1);      // move left N columns (default N=1)
inline void next_line(const std::size_t& = 1); // move to the beginning of next line, N lines down (default N=1)
inline void prev_line(const std::size_t& = 1); // move to the beginning of previous line, N lines up (default N=1)
inline void home();                            // home cursor to (0, 0)
inline void position_report();                 // generate a cursor position report
inline void hide();                            // hide cursor
inline void show();                            // show cursor

inline void move(const std::size_t&, const std::size_t&); // move cursor by the given row and column
inline void set(const std::size_t&, const std::size_t&);  // move cursor to given row and column
inline void set(const cursor_pos_t&);
inline cursor_pos_t position();                           // returns the current cursor position (row, column)
} // namespace Cursor


// end --- cursor.h --- 



// begin --- screen.h --- 

#pragma once

namespace Screen {
struct Size {
    std::size_t rows;
    std::size_t columns;
};

inline void clear();         // clear screen
inline void clear_to_eol();  // clear from cursor position to the end of the line
inline void clear_to_eof();  // clear from cursor position to the end of the screen
inline void clear_to_sol();  // clear from cursor position to the start of the line
inline void clear_to_sof();  // clear from cursor position to the start of the screen
inline void clear_line();    // clear the entire line where the cursor is located

// clear from a specific (Y, X) and a custom (width, height) *idea from Newtrodit*
inline void clear_partial(const std::size_t&, const std::size_t&, const std::size_t&, const std::size_t&); 

void save();         // save screen state
void restore();      // restore last saved screen state
Screen::Size size(); // get terminal screen size
} // namespace Screen 


// end --- screen.h --- 



#include <cerrno>
#include <cstdio>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <termios.h>

inline void Screen::clear()         { std::cerr << "\033[2J\033[H" << std::flush; }
inline void Screen::clear_to_eol()  { std::cerr << "\033[0K" << std::flush; }
inline void Screen::clear_to_eof()  { std::cerr << "\033[0J" << std::flush; }
inline void Screen::clear_to_sol()  { std::cerr << "\033[1K" << std::flush; }
inline void Screen::clear_to_sof()  { std::cerr << "\033[1J" << std::flush; }
inline void Screen::clear_line()    { std::cerr << "\033[2K" << std::flush; }
inline void Screen::clear_partial(const std::size_t& row, const std::size_t& column, const std::size_t& width, const std::size_t& height) {
    cursor_pos_t oldpos = Cursor::position();
    std::string spaces(width, ' ');

    for (int i = 0; i < height; i++) {
        Cursor::set(row + i, column);
        std::cout << spaces;
    }
    Cursor::set(oldpos.row, oldpos.column);
}



//void Screen::save()         { std::cerr << "\033[?1049h" << std::flush; }
//void Screen::restore()      { std::cerr << "\033[?1049l" << std::flush; }
void Screen::save()         { std::cerr << "\033[?47h" << std::flush; }
void Screen::restore()      { std::cerr << "\033[?47l" << std::flush; }
Screen::Size Screen::size() {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
        throw Term::Exception("Couldn't get terminal size. " + std::string(Term::is_stdout_a_tty() ? "" : "(STDOUT IS NOT A TTY!)"));
    else {
        Screen::Size term_size;
        term_size.rows = ws.ws_row;
        term_size.columns = ws.ws_col;
        return term_size;
    }
}


// end --- screen.cpp --- 



// begin --- color.cpp --- 



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

enum class Style : std::uint8_t {
    RESET         = 0,
    BOLD          = 1,
    DIM           = 2,
    ITALIC        = 3,
    UNDERLINE     = 4,
    BLINK         = 5,
    REVERSE       = 6,
    CONCEAL       = 7,
    STRIKETHROUGH = 8, // mostly supported
    OVERLINE      = 53 // barely supported 
};

// class for representing 24bit color
class rgb {
public:
    rgb() = default;
    rgb(const std::uint8_t& red, const std::uint8_t& green, const std::uint8_t& blue) : r(red), g(green), b(blue), empty(false) {}
    
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
ColorBit4 rgb_to_bit4(std::uint8_t, std::uint8_t, std::uint8_t);
uint8_t rgb_to_bit8(rgb);
uint8_t rgb_to_bit8(std::uint8_t, std::uint8_t, std::uint8_t);

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



// end --- color.cpp --- 



// begin --- term.cpp --- 



#include <cerrno>
#include <cstdio>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <termios.h>

static struct termios original_termios;
static bool __is_raw_mode_enabled = false; // weird name to avoid possible conflict

/********************* NAMESPACE PRIVATE *********************/
std::string Term::Private::getenv(const std::string& env) {
    if (std::getenv(env.c_str()) != nullptr) 
        return static_cast<std::string>(std::getenv(env.c_str()));
    else 
        return std::string();
}
/*************************************************************/


bool Term::is_a_tty(const FILE* fd) { return isatty(fileno(const_cast<FILE*>(fd))); }
bool Term::is_stdin_a_tty()         { return is_a_tty(stdin); }
bool Term::is_stdout_a_tty()        { return is_a_tty(stdout); }
bool Term::is_stderr_a_tty()        { return is_a_tty(stderr); }
termios Term::get_term_attributes() {
    struct termios term;
    if (tcgetattr(STDIN_FILENO, &term) != 0)
        throw Term::Exception("tcgetattr() failed");
    return term;
}

inline void Term::enable_raw_mode() {
    if (__is_raw_mode_enabled)
        return;
    struct termios raw;

    tcgetattr(STDIN_FILENO, &original_termios);
    raw = original_termios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_cflag |= (CS8);
    //raw.c_oflag &= ~(OPOST);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    __is_raw_mode_enabled = true;
}

inline void Term::disable_raw_mode() {
    if (!__is_raw_mode_enabled)
        return;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
    __is_raw_mode_enabled = false;
}

inline bool Term::is_raw_mode() {
    struct termios current_termios;
    tcgetattr(STDIN_FILENO, &current_termios);

    return __is_raw_mode_enabled && (current_termios.c_lflag & ICANON) == 0;
}


bool Term::stdin_connected()  { return Term::is_stdin_a_tty(); }
bool Term::stdout_connected() { return Term::is_stdout_a_tty(); }

//void Term::screen_save()                            { std::cerr << "\0337\033[?1049h" << std::flush; }
//void Term::screen_load()                            { std::cerr << "\033[?1049l\0338" << std::flush; }
void Term::enter_alt_buffer()                       { std::cerr << "\033[?1049h" << std::flush; }
void Term::exit_alt_buffer()                        { std::cerr << "\033[?1049l" << std::flush; }
void Term::terminal_title(const std::string& title) { std::cerr << "\033]0;" + title + '\a' << std::flush; }


// end --- term.cpp --- 



// begin --- cursor.cpp --- 



inline void Cursor::set_row(const std::size_t& row) {
    cursor_pos_t oldpos = Cursor::position(); 
    Cursor::set(row, oldpos.column);
}
inline void Cursor::set_column(const std::size_t& column) {
    std::cerr << "\033[" << column << 'G' << std::flush;
}
inline void Cursor::up(const std::size_t& lines) {
    std::cerr << "\033[" << lines << 'A' << std::flush;
}
inline void Cursor::down(const std::size_t& lines) {
    std::cerr << "\033[" << lines << 'B' << std::flush;
}
inline void Cursor::right(const std::size_t& lines) {
    std::cerr << "\033[" << lines << 'C' << std::flush;
}
inline void Cursor::left(const std::size_t& lines) {
    std::cerr << "\033[" << lines << 'D' << std::flush;
}
inline void Cursor::next_line(const std::size_t& lines) {
    std::cerr << "\033[" << lines << 'E' << std::flush;
}
inline void Cursor::prev_line(const std::size_t& lines) {
    std::cerr << "\033[" << lines << 'F' << std::flush;
}
inline void Cursor::home() {
    std::cerr << "\033[H" << std::flush;
}
inline void Cursor::position_report() {
    std::cerr << "\033[6n" << std::flush;
}
inline void Cursor::hide() {
    std::cerr << "\033[?25l" << std::flush;
}
inline void Cursor::show() {
    std::cerr << "\033[?25h" << std::flush;
}
inline void Cursor::move(const std::size_t& rows, const std::size_t& columns) {
    if      (rows > 0) { Cursor::down(rows); }
    else if (rows < 0) { Cursor::up(rows); }
    
    if      (columns > 0) { Cursor::right(columns); }
    else if (columns < 0) { Cursor::left(columns); }
}
inline void Cursor::set(const std::size_t& row, const std::size_t& column) {
    std::cerr << "\033[" << row << ';' << column << 'H' << std::flush;
}
inline void Cursor::set(const cursor_pos_t& pos) {
    Cursor::set(pos.row, pos.column);
}

inline cursor_pos_t Cursor::position() {
    struct termios term;
    cursor_pos_t cursor_pos;

    // Get the terminal attributes
    tcgetattr(STDIN_FILENO, &term);

    // Save the current terminal attributes
    struct termios saved_term = term;

    // Disable canonical mode and echo mode
    term.c_lflag &= ~(ICANON | ECHO);

    // Set the new terminal attributes
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

    // Get the cursor position
    std::cerr << "\033[6n" << std::flush;
    char buf[32];
    int i = 0;
    while (i < 32) {
        char c;
        if (read(STDIN_FILENO, &c, 1) != 1) {
            break;
        }
        buf[i++] = c;
        if (c == 'R') {
            break;
        }
    }
    buf[i] = '\0';

    // Parse the cursor position
    int row, column;
    if (sscanf(buf, "\033[%d;%dR", &row, &column) != 2) {
        throw Term::Exception("Failed to parse cursor position\n");
    }

    cursor_pos.row = row;
    cursor_pos.column = column;

    // Restore the original terminal attributes
    tcsetattr(STDIN_FILENO, TCSANOW, &saved_term);

    return cursor_pos;
}


// end --- cursor.cpp --- 

