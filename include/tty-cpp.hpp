

// begin --- color.cpp --- 



// begin --- term.h --- 

#pragma once

#include <cstdint>
#include <string>
#include <exception>
#include <stdexcept>
#include <unistd.h>
#include <termios.h>

struct cursor_pos_t {
    int row;
    int column;
};

namespace Term {
const std::string Version = "1.0.0";
const std::string Repo = "https://github.com/ZackeryRSmith/tty-cpp";

/********************* NAMESPACE PRIVATE *********************/
namespace Private {
  std::string getenv(const std::string&);

  // returns the terminal size as (rows, columns) / (Y, X), throws a runtime error
  // if the console is not connected
  std::pair<std::size_t, std::size_t> get_term_size();
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

termios get_term_attributes();
bool is_a_tty(const FILE*);
bool is_stdin_a_tty();
bool is_stdout_a_tty();
bool is_stderr_a_tty();

// clear functions
inline void clear_screen();  // clear screen
inline void clear_to_eol();  // clear from cursor position to the end of the line
inline void clear_to_eof();  // clear from cursor position to the end of the screen
inline void clear_to_sol();  // clear from cursor position to the start of the line
inline void clear_to_sof();  // clear from cursor position to the start of the screen
inline void clear_line();    // clear the entire line where the cursor is located

// clear from a specific (Y, X) and a custom (width, height) *idea from Newtrodit*
inline void clear_partial(const std::size_t&, const std::size_t&, const std::size_t&, const std::size_t&); 

std::pair<std::size_t, std::size_t> get_size(); // get terminal size (column, row)
bool stdin_connected(); // check if stdin is connected to a TTY
bool stdout_connected(); // check if stdout is connect to a TTY

// terminal cursor control 
inline void set_row(const std::size_t&);        // move cursor to row       
inline void set_column(const std::size_t&);     // move cursor to column
inline void cursor_up(const std::size_t& = 1);    // move the cursor up N lines (default N=1)
inline void cursor_down(const std::size_t& = 1);  // move the cursor down N lines (default N=1)
inline void cursor_right(const std::size_t& = 1); // move the cursor forward N columns (default N=1)
inline void cursor_left(const std::size_t& = 1);  // move the cursor backward N columns (default N=1)
inline void cursor_next(const std::size_t& = 1);  // moves cursor to beginning of next line, N lines down (default N=1)
inline void cursor_prev(const std::size_t& = 1);  // moves cursor to beginning of previous line, N lines up (default N=1)
inline void cursor_home();                        // home cursor to (0, 0)
inline void cursor_position_report();           // the ANSI code to generate a cursor position report
inline void cursor_off();                       // hide cursor
inline void cursor_on();                        // show cursor

inline void cursor_move(const std::size_t&, const std::size_t&); // move cursor by the given row and column
inline void cursor_set(const std::size_t&, const std::size_t&);  // move cursor to given row and column
inline void cursor_set(const cursor_pos_t&);
inline cursor_pos_t cursor_position();                           // returns the current cursor position (row, column)

void screen_save();                            // save screen state
void screen_load();                            // restore last saved screen state
void enter_alt_buffer();                       // enter the alternate terminal buffer
void exit_alt_buffer();                        // exit the alternate terminal buffer
void terminal_title(const std::string& title); // change the terminal title (supported by only a few terminals)

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



// begin --- term.cpp --- 



#include <cerrno>
#include <cstdio>
#include <string>
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
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
        throw Term::Exception("Couldn't get terminal size. Are we even connected to a TTY?");
    else
        return std::pair<std::size_t, std::size_t>(ws.ws_col, ws.ws_row);
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

std::pair<std::size_t, std::size_t> Term::get_size() { return Private::get_term_size(); }
bool Term::stdin_connected() { return Term::is_stdin_a_tty(); }
bool Term::stdout_connected() { return Term::is_stdout_a_tty(); }

inline void Term::clear_screen()  { std::cerr << "\033[2J\033[H" << std::flush; }
inline void Term::clear_to_eol()  { std::cerr << "\033[0K" << std::flush; }
inline void Term::clear_to_eof()  { std::cerr << "\033[0J" << std::flush; }
inline void Term::clear_to_sol()  { std::cerr << "\033[1K" << std::flush; }
inline void Term::clear_to_sof()  { std::cerr << "\033[1J" << std::flush; }
inline void Term::clear_line()    { std::cerr << "\033[2K" << std::flush; }
inline void Term::clear_partial(const std::size_t& row, const std::size_t& column, const std::size_t& width, const std::size_t& height) {
    cursor_pos_t oldpos = Term::cursor_position();
    std::string spaces(width, ' ');

    for (int i = 0; i < height; i++) {
        Term::cursor_set(row + i, column);
        std::cout << spaces;
    }
    Term::cursor_set(oldpos.row, oldpos.column);
}

inline void Term::set_row(const std::size_t& row) {
    cursor_pos_t oldpos = Term::cursor_position(); Term::cursor_set(row, oldpos.column);
}
inline void Term::set_column(const std::size_t& column) {
    std::cerr << "\033[" << column << 'G' << std::flush;
}
inline void Term::cursor_up(const std::size_t& lines) {
    std::cerr << "\033[" << lines << 'A' << std::flush;
}
inline void Term::cursor_down(const std::size_t& lines) {
    std::cerr << "\033[" << lines << 'B' << std::flush;
}
inline void Term::cursor_right(const std::size_t& lines) {
    std::cerr << "\033[" << lines << 'C' << std::flush;
}
inline void Term::cursor_left(const std::size_t& lines) {
    std::cerr << "\033[" << lines << 'D' << std::flush;
}
inline void Term::cursor_next(const std::size_t& lines) {
    std::cerr << "\033[" << lines << 'E' << std::flush;
}
inline void Term::cursor_prev(const std::size_t& lines) {
    std::cerr << "\033[" << lines << 'F' << std::flush;
}
inline void Term::cursor_home() {
    std::cerr << "\033[H" << std::flush;
}
inline void Term::cursor_position_report() {
    std::cerr << "\033[6n" << std::flush;
}
inline void Term::cursor_off() {
    std::cerr << "\033[?25l" << std::flush;
}
inline void Term::cursor_on() {
    std::cerr << "\033[?25h" << std::flush;
}
inline void Term::cursor_move(const std::size_t& rows, const std::size_t& columns) {
    if      (rows > 0) { Term::cursor_down(rows); }
    else if (rows < 0) { Term::cursor_up(rows); }
    
    if      (columns > 0) { Term::cursor_right(columns); }
    else if (columns < 0) { Term::cursor_left(columns); }
}
inline void Term::cursor_set(const std::size_t& row, const std::size_t& column) {
    std::cerr << "\033[" << row << ';' << column << 'H' << std::flush;
}
inline void Term::cursor_set(const cursor_pos_t& pos) { Term::cursor_set(pos.row, pos.column); }

/* for the love of all gods optimise this code */
inline cursor_pos_t Term::cursor_position() {
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
    if (sscanf(buf, "\033[%d;%dR", &cursor_pos.row, &cursor_pos.column) != 2) {
        throw Term::Exception("Failed to parse cursor position\n");
    }

    // Restore the original terminal attributes
    tcsetattr(STDIN_FILENO, TCSANOW, &saved_term);

    return cursor_pos;
}

//void Term::screen_save()                            { std::cerr << "\0337\033[?1049h" << std::flush; }
//void Term::screen_load()                            { std::cerr << "\033[?1049l\0338" << std::flush; }
void Term::screen_save()                            { std::cerr << "\033[?47h" << std::flush; }
void Term::screen_load()                            { std::cerr << "\033[?47l" << std::flush; }
void Term::enter_alt_buffer()                       { std::cerr << "\033[?1049h" << std::flush; }
void Term::exit_alt_buffer()                        { std::cerr << "\033[?1049l" << std::flush; }
void Term::terminal_title(const std::string& title) { std::cerr << "\033]0;" + title + '\a' << std::flush; }


// end --- term.cpp --- 

