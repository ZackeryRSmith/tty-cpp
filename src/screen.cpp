#include "headers/term.h"
#include "headers/cursor.h"
#include "headers/screen.h"

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
