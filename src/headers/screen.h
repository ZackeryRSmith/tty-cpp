#pragma once

namespace Screen {
struct Size {
    std::size_t rows;
    std::size_t columns;
};

inline void clear();         // clear screen
inline void clear_to_eol();  // clear from cursor position to the end of the line
inline void clear_to_eof();  // clear from cursor position to the end of the screen
inline void clear_to_sol();  // clear from cursor position to the start of the line
inline void clear_to_sof();  // clear from cursor position to the start of the screen
inline void clear_line();    // clear the entire line where the cursor is located

// clear from a specific (Y, X) and a custom (width, height) *idea from Newtrodit*
inline void clear_partial(const std::size_t&, const std::size_t&, const std::size_t&, const std::size_t&); 

void save();         // save screen state
void restore();      // restore last saved screen state
Screen::Size size(); // get terminal screen size
} // namespace Screen 
