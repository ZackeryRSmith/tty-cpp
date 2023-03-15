// doesn't really use ttycpp this is just some random testing
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <csignal>
#include "../include/tty-cpp.hpp"

enum class Key {
    UNKNOWN = -1,
    // ASCII values for control characters
    NUL = 0,
    SOH = 1,
    STX = 2,
    ETX = 3,
    EOT = 4,
    ENQ = 5,
    ACK = 6,
    BEL = 7,
    BS = 8,
    HT = 9,
    LF = 10,
    VT = 11,
    FF = 12,
    CR = 13,
    SO = 14,
    SI = 15,
    DLE = 16,
    DC1 = 17,
    DC2 = 18,
    DC3 = 19,
    DC4 = 20,
    NAK = 21,
    SYN = 22,
    ETB = 23,
    CAN = 24,
    EM = 25,
    SUB = 26,
    ESC = 27,
    FS = 28,
    GS = 29,
    RS = 30,
    US = 31,
    // Regular characters (ASCII values 32 to 126)
    SPACE = 32,
    EXCLAMATION_MARK = 33,
    // ...
    TILDE = 126,
    // CTRL keys combinations
    CTRL_A = 128,
    CTRL_B,
    CTRL_C,
    CTRL_D,
    CTRL_E,
    CTRL_F,
    CTRL_G,
    CTRL_H,
    CTRL_I,
    CTRL_J,
    CTRL_K,
    CTRL_L,
    CTRL_M,
    CTRL_N,
    CTRL_O,
    CTRL_P,
    CTRL_Q,
    CTRL_R,
    CTRL_S,
    CTRL_T,
    CTRL_U,
    CTRL_V,
    CTRL_W,
    CTRL_X,
    CTRL_Y,
    CTRL_Z,
    // Special keys
    ENTER,
    UP_ARROW,
    DOWN_ARROW,
    LEFT_ARROW,
    RIGHT_ARROW,
    CTRL_BASE,
    // Modifier keys
    ALT,
    SHIFT,
    // Add more keys as needed
};

int enable_raw_mode() {
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

int disable_raw_mode() {
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

bool is_raw_mode() {
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

Key getch() {
    bool disable_raw = !is_raw_mode(); // disable raw on function return?
    if (disable_raw)
        disable_raw = true; 
        enable_raw_mode();
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

int main() {
    while (true) {
        Key pressed_key = getch();

        if (pressed_key == Key::CTRL_A) {
            break; // Exit the loop
        } else if (pressed_key == Key::UNKNOWN) {
            break; // Exit the loop if an unknown key is pressed
        }
    }
    
    return 0;
}

