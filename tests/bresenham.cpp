// Bresenham's line algorithm
#include "../include/tty-cpp.hpp"
#include <iostream>
#include <string>
#include <cmath>

void plot_line_low(int x0, int y0, 
                   int x1, int y1,
                   const std::string& ch) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int yi = 1;

    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    int D = (2 * dy) - dx;
    int y = y0;

    for (int x = x0; x < x1; x++) {
        Term::cursor_set(y, x);
        std::cout << ch << std::flush;

        if (D > 0) {
            y += yi;
            D += 2 * (dy - dx);
        } else {
            D += 2 * dy;
        }
    }
}

void plot_line_high(int x0, int y0, 
                    int x1, int y1, 
                    const std::string& ch) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int xi = 1;
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }
    int D = (2 * dx) - dy;
    int x = x0;
    
    for (int y = y0; y < y1; y++) {
        Term::cursor_set(y, x);
        std::cout << ch << std::flush;
        
        if (D > 0) {
            x += xi;
            D += 2 * (dx - dy);
        } else {
            D += 2 * dx;
        }
    }
}


void plot_line(int x0, int y0, 
               int x1, int y1,
               const std::string& ch = "#") {
    cursor_pos_t oldpos = Term::cursor_position();

    if (abs(int(y1 - y0)) < abs(int(x1 - x0))) {
        if (x0 > x1) {
            plot_line_low(x1+oldpos.column, y1+oldpos.row, x0+oldpos.column, y0+oldpos.row, ch);
        } else {
            plot_line_low(x0+oldpos.column, y0+oldpos.row, x1+oldpos.column, y1+oldpos.row, ch);
        }
    } else {
        if (y0 > y1) {
            plot_line_high(x1+oldpos.column, y1+oldpos.row, x1+oldpos.column, y0+oldpos.row, ch);
        } else {
            plot_line_high(x0+oldpos.column, y0+oldpos.row, x1+oldpos.column, y1+oldpos.row, ch);
        }
    }

    Term::cursor_set(oldpos);
}

int main() {
    try {
    Term::clear_screen();

    // tty-cpp version and link to the repo
    std::cout << Term::style(Term::Style::BOLD) << "Running tty-cpp version: " << Term::style(Term::Style::RESET) << Term::Version 
              << Term::style(Term::Style::BOLD) << "\nRepo: " << Term::style(Term::Style::RESET) << Term::Repo 
              << std::endl << std::endl;

    if (Term::stdout_connected()) 
        std::cout << Term::color_fg(Term::ColorBit4::GREEN) << "Standard output is attached to a terminal." 
                  << std::endl << std::endl;
    else
        std::cout << Term::color_fg(Term::ColorBit4::RED) << "Standard output is not attached to a terminal." 
                  << std::endl << std::endl;
    
    std::cout << Term::color_fg(Term::ColorBit4::DEFAULT);

    plot_line(0, 5, 10, 5);
    plot_line(5, 0, 5, 11);

    Term::cursor_next(10);
    std::cout << std::endl;

    } catch (const Term::Exception& re) {
        std::cerr << "tty-cpp error: " << re.what() << std::endl;
        return 2;
    } catch (...) {
        std::cerr << "Unknown error." << std::endl;
        return EXIT_FAILURE;
    }
}
