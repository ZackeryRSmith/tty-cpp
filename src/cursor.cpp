#include "headers/term.h"
#include "headers/cursor.h"

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
