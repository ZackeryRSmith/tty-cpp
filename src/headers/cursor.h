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
