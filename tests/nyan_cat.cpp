// Based on https://github.com/djui/nyancat

#include <iostream>
#include <unistd.h>
#include "../include/tty-cpp.hpp"

#define DELAY 20000
#define WIDTH 60
#define ANGLE 2
#define FLAG "`*.,*'^"

static const char* CAT[] = {
    "         ",
    "         ",
    "         ",
    "         ",
    " ,---/V\\ ",
    "~|__(o.o)",
    " U U U U ",
    "         ",
    "         ",
    "         ",
    "         ",
    " ,---/V\\ ",
    "~|__(o.o)",
    "  UU  UU "
};

static const Term::rgb COLORS[] = {
    Term::rgb(255,   0,   0), // red
    Term::rgb(255, 165,   0), // orange
    Term::rgb(255, 255,   0), // yellow
    Term::rgb(  0, 255,   0), // green
    Term::rgb(  0,   0, 255), // blue
    Term::rgb( 75,   0, 130), // indigo
    Term::rgb(148,   0, 211)  // violet
};

int main() {
    RawModeGuard raw_mode_guard;
    Cursor::hide();

    std::cout << Term::style(Term::Style::BOLD) << "Running tty-cpp version: " << Term::style(Term::Style::RESET) << Term::VERSION 
              << Term::style(Term::Style::BOLD) << "\nRepo: " << Term::style(Term::Style::RESET) << Term::REPO 
              << std::endl << std::endl;

    if (Term::stdout_connected()) 
        std::cout << Term::color_fg(Term::ColorBit4::GREEN) << "Standard output is attached to a terminal." << std::endl << std::endl;
    else
        std::cout << Term::color_fg(Term::ColorBit4::RED) << "Standard output is not attached to a terminal." << std::endl << std::endl;
    std::cout << Term::color_fg(Term::ColorBit4::DEFAULT);

    int i                = 0;
    const int COLORS_LEN = sizeof(COLORS) / sizeof(Term::rgb);
    const int FLAG_LEN   = sizeof(FLAG)   / sizeof(char) - 1;

    // Animation
    while (true) {
        for (int y = 0; y < COLORS_LEN; y++) {
            /*-- line --*/
            std::cout << Term::color_fg(COLORS[y]);
            for (int x = 0; x < WIDTH - ANGLE * (COLORS_LEN - y); x++)
                std::cout << FLAG[(x + (FLAG_LEN - y) + i) % FLAG_LEN];
            std::cout << Term::color_fg(Term::ColorBit4::DEFAULT);

            // hold the line to the angle
            for (int t = ANGLE; t < ANGLE * (COLORS_LEN - y); t++)
                std::cout << ' ';
            /************/

            /*-- cat --*/
            std::cout << Term::style(Term::Style::BOLD)
                      << CAT[y % COLORS_LEN + (i % 10 < COLORS_LEN ? 0 : COLORS_LEN)]
                      << Term::style(Term::Style::RESET) << std::endl;
            /***********/
        }

        if (Term::keyhit()) { // key pressed?
            Cursor::show();
            break;
        }

        i++;
        usleep(DELAY); // wait x ms
        Cursor::up(COLORS_LEN); // move up before looping
    }

    std::cout << Term::color_fg(Term::ColorBit4::DEFAULT);
    return 0;
}

