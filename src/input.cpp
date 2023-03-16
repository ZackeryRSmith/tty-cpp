#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <csignal>

#include "term.h"
#include "input.h"

Key Term::getkey() {
    // disable raw mode on function return?
    bool disable_raw = !is_raw_mode(); 
    if (disable_raw) enable_raw_mode();
    
    char c;

    if (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == '\x1b') {
            char seq[3];

            if (read(STDIN_FILENO, &seq[0], 1) == 1 && read(STDIN_FILENO, &seq[1], 1) == 1) {
                if (disable_raw) disable_raw_mode();
                if (seq[0] == '[') {
                    switch (seq[1]) {
                        case 'A': return Key::UP_ARROW;
                        case 'B': return Key::DOWN_ARROW;
                        case 'C': return Key::RIGHT_ARROW;
                        case 'D': return Key::LEFT_ARROW;
                    }
                }
            }
        } else if (c == '\r') {
            if (disable_raw) disable_raw_mode();
            return Key::ENTER;
        } else if (c >= 1 && c <= 26) {
            if (disable_raw) disable_raw_mode();
            return static_cast<Key>(static_cast<int>(Key::CTRL_A) + (c - 1));
        } else {
            if (disable_raw) disable_raw_mode();
            return static_cast<Key>(c);
        }
    }

    if (disable_raw) disable_raw_mode();
    return Key::UNKNOWN;
}
