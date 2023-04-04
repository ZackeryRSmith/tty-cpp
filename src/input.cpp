#include "headers/term.h"
#include "headers/input.h"
#include <fcntl.h>
#include <sys/ioctl.h>

Key Term::getkey() {
    char c;

    if (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == '\x1b') {
            char seq[4];

            if (read(STDIN_FILENO, &seq[0], 1) != 1) return Key::ESC;
            if (read(STDIN_FILENO, &seq[1], 1) != 1) return Key::ESC;

            if (seq[0] == '[') {
                if (seq[1] >= '0' && seq[1] <= '9') {
                    if (seq[2] == '~') {
                        switch (seq[1]) {
                            case '3': return Key::DEL;
                        }
                    }
                } else {
                    switch (seq[1]) {
                        case 'A': return Key::UP_ARROW;
                        case 'B': return Key::DOWN_ARROW;
                        case 'C': return Key::RIGHT_ARROW;
                        case 'D': return Key::LEFT_ARROW;
                    }
                }
            }
        } else if (c == '\r') {
            return Key::ENTER;
        } else if (c >= 1 && c <= 26) {
            return static_cast<Key>(static_cast<int>(Key::CTRL_A) + (c - 1));
        } else {
            return static_cast<Key>(c);
        }
    }

    return Key::UNKNOWN;
}

int Term::keyhit() {
    int bytesWaiting;
    ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);

    return bytesWaiting;
}
