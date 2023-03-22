#include "headers/term.h"

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
