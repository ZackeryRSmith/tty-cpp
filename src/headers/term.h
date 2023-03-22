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

