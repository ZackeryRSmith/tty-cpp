#include "term.h"

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

inline int Term::enable_raw_mode() {
    int fd = STDIN_FILENO;
    struct termios term;
    if (tcgetattr(fd, &term) == -1)
        return -1;
    term.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    term.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    term.c_cflag &= ~(CSIZE | PARENB);
    term.c_cflag |= CS8;
    term.c_oflag &= ~(OPOST);
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;
    if (tcsetattr(fd, TCSAFLUSH, &term) == -1)
        return -1;
    return 0;
}

inline int Term::disable_raw_mode() {
    int fd = STDIN_FILENO;
    struct termios term;
    if (tcgetattr(fd, &term) == -1)
        return -1;
    term.c_lflag |= (ECHO | ICANON | IEXTEN | ISIG);
    term.c_iflag |= (BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    term.c_cflag |= (CSIZE | PARENB);
    term.c_cflag &= ~CS8;
    term.c_oflag |= (OPOST);
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;
    if (tcsetattr(fd, TCSAFLUSH, &term) == -1)
        return -1;
    return 0;
}

inline bool Term::is_raw_mode() {
    int fd = STDIN_FILENO;
    struct termios term;

    if (tcgetattr(fd, &term) == -1)
        return false;

    // check if the terminal settings match raw mode conditions
    bool is_raw = !((term.c_lflag & (ECHO | ICANON | IEXTEN | ISIG)) &&
                   (term.c_iflag & (BRKINT | ICRNL | INPCK | ISTRIP | IXON)) &&
                   (term.c_cflag & (CSIZE | PARENB)) &&
                   !(term.c_cflag & CS8) &&
                   (term.c_oflag & OPOST));

    return is_raw;
}

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
