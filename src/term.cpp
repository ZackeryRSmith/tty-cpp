#include "term.h"

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
