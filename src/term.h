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

