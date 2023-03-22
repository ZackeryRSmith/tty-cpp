// choose fun
#include "../include/tty-cpp.hpp"
#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
#include <vector>

void open() {
    Screen::clear();
    Cursor::hide();
}

void die(int status = 0) {
    Cursor::show();
    Screen::clear();
    exit(status);
}

void box(int row,   int column,
         int width, int height,
         std::string topLeft     = "┌",
         std::string topRight    = "┐",
         std::string bottomLeft  = "└",
         std::string bottomRight = "┘",
         std::string dash        = "─",
         std::string pipe        = "│") {
    cursor_pos_t oldpos = Cursor::position();

    std::string spaces  (width-2, ' ');

    for (int i = 0; i < height; i++) {
        Cursor::set(row + i, column);
        if (i == 0) {
            std::cout << topLeft;
            for (int j = 2; j < width; j++) std::cout << dash;
            std::cout << topRight;
        } else if (i == height-1) {
            std::cout << bottomLeft;
            for (int j = 2; j < width; j++) std::cout << dash;
            std::cout << bottomRight;
        } else { 
            std::cout << pipe << spaces << pipe;
        }
    }
    Cursor::set(oldpos);
}

void alert(const std::string& type, const std::string& action, const std::string& msg) {
    RawModeGuard raw_mode_guard;
    std::vector<std::string> split_msg;
    std::istringstream iss(msg);
    std::string line;

    while (std::getline(iss, line)) 
        split_msg.push_back(line);

    auto longest_string = std::max_element(split_msg.begin(), split_msg.end(),
        [](const std::string& a, const std::string& b) {
            return a.size() < b.size();
        });

    int width = longest_string->size() + 4;
    int height = split_msg.size() + 4;

    Screen::Size term_size = Screen::size();
    int x = term_size.columns / 2 - width / 2;
    int y = term_size.rows / 2 - height / 2;
    
    Screen::save(); // BUG: for some reason doing this stops overlapping stuff

    box(y, x, width, height);

    Cursor::set(y, x + 1);
    std::cout << "┤ " << Term::style(Term::Style::BOLD) 
              << type 
              << Term::style(Term::Style::RESET) << " ├";

    for (int line = 0; line < split_msg.size(); line++) {
        Cursor::set((y + 2) + line, x + 2);
        std::cout << split_msg[line];
    }

    Cursor::set(y + (height - 1), (x + floor(width / 2) - floor(button_text.length() / 2))-1);
    std::cout << "┤"
              << Term::style(Term::Style::REVERSE) 
              << action 
              << Term::style(Term::Style::RESET) 
              << "├" 
              << std::flush;
    
    while (true) {
        Key pressed_key = Term::getkey();

        if (pressed_key == Key::ENTER) {
            Screen::restore();
            break;
        } else if (pressed_key == Key::CTRL_C) {
            die();
        }
    }
}

