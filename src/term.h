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

