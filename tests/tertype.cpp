/* TerType -- A very simple clone of TypeRacer. */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
//#include <chrono>
#include <thread>
#include <mutex>
#include "../include/tty-cpp.hpp"

#define VERSION "0.1"

#define BGFILLCOLOR Term::ColorBit4::BLACK
#define FGBORDERCOLOR Term::ColorBit4::WHITE
#define CURRENTCHARCOLOR Term::ColorBit4::RED
#define BEFORCHARCOLOR Term::ColorBit4::WHITE
#define AFTERCHARCOLOR Term::ColorBit4::GRAY

/**
 * @brief Fills the screen with a given character and color.
 *
 * @param c The character to fill the screen with.
 * @param color The color to use for filling the screen. Default value is no color.
 *
 * @details This function fills the entire screen with the specified character and color (if provided).
 *
 * Usage:
 *  - To fill the screen with a character and color:
 *      - fill_screen('x', Term::rgb(255, 0, 0)); // fills the screen with red 'x'
 *      - fill_screen('-', Term::ColorBit4::Blue); // fills the screen with blue '-'
 *  - To fill the screen with a character and no color:
 *      - fill_screen('*'); // fills the screen with '*' in the default background color
 *
 */
void fill_screen(char c, std::variant<std::monostate, Term::rgb, Term::ColorBit4> color = std::variant<std::monostate, Term::rgb, Term::ColorBit4>{}) {
    Screen::clear();
    Screen::Size term_size = Screen::size();
    
    std::string fill_color = "";

    if (!std::holds_alternative<std::monostate>(color)) {
        if (std::holds_alternative<Term::rgb>(color)) {
            fill_color = Term::color_bg(std::get<Term::rgb>(color));
        } else {
            fill_color = Term::color_bg(std::get<Term::ColorBit4>(color));
        }
    }

    for (int y = 0; y < term_size.columns; y++) {
        for (int x = 0; x < term_size.rows; x++) {
            std::cout << fill_color << c;
        }
    }

    Cursor::home();
}

/**
 * @brief Fills the screen with a given character and RGB color.
 *
 * @param c The character to fill the screen with.
 * @param r The red component of the RGB color.
 * @param g The green component of the RGB color.
 * @param b The blue component of the RGB color.
 *
 * @details This function fills the entire screen with the specified character and RGB color.
 *
 * Usage:
 *  - To fill the screen with a character and RGB color:
 *      - fill_screen('x', 255, 0, 0); // fills the screen with red 'x'
 *      - fill_screen('-', 0, 0, 255); // fills the screen with blue '-'
 *
 */
void fill_screen(char c, size_t r, size_t g, size_t b) { 
    fill_screen(c, Term::rgb(r, g, b)); 
}

std::string repeat_str(const std::string& str, const size_t n) {
    std::string result;
    result.reserve(n * str.length());
    
    for (size_t i = 0; i < n; ++i) {
        result += str;
    }
    
    return result;
}

int _word_count(const std::string& s) {
    int word_count = 1;
    
    if(std::all_of(s.begin(),s.end(),isspace))
        return 0;

    for (int i = 0; i < s.size(); i++) {
        if (i - 1 >= 0 && i+1 < s.size())
            if (s.at(i) == ' ' && s.at(i + 1) != ' ')
                word_count++;
    }

    return word_count;
}

void safe_exit() {
    Cursor::show();
    exit(0);
}

class Typer {
    public:
        Typer(std::string _text) : text(_text) {
            word_count = _word_count(text);
            //display_wpm();
            //last_word_time = std::chrono::steady_clock::now();
        }

        void handle_key(Key key) {
            std::lock_guard<std::mutex> lock(display_mutex); // lock mutex
            
            if (pointer < text.size() && text.at(pointer) == static_cast<char>(key)) {
                if (pointer == text.size() - 1)
                    safe_exit();

                if (pointer >= round(term_size.columns / 2))
                    spos++;


                if (text.at(pointer) == ' ') {
                    words_done++;
                //    auto now = std::chrono::steady_clock::now();
                //    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_word_time);
                //    last_word_time = now;
                //    wpm = words_done / (elapsed_time.count() / 60000.0); // words per minute
                }
            
                epos++;
                pointer++;
            }
        }

        void display() {
            std::lock_guard<std::mutex> lock(display_mutex); // lock mutex
            
            Cursor::set(round(term_size.rows / 2) - 2, 0);

            std::cout << Term::color_fg(FGBORDERCOLOR) << Term::color_bg(BGFILLCOLOR);
            std::cout << repeat_str("─", round(term_size.columns / 2)) << "┬" << repeat_str("─", round(term_size.columns / 2)) << std::endl;
            Cursor::next_line();
            std::cout << repeat_str("─", round(term_size.columns / 2)) << "┴" << repeat_str("─", round(term_size.columns / 2));
            Cursor::prev_line();

            std::cout << repeat_str(" ", round(term_size.columns / 2) - ((pointer) - spos)) // space before text 
                      /* TEXT BEFORE CHAR */
                      << Term::color_fg(BEFORCHARCOLOR)
                      << text.substr(spos, (pointer) - spos)
                      << Term::style(Term::Style::RESET)
                      /********************/

                      /* ON CHAR */
                      << (text.at(pointer) == ' ' ? Term::color_bg(CURRENTCHARCOLOR) : Term::color_fg(CURRENTCHARCOLOR)) 
                      << Term::style(Term::Style::BOLD)
                      << text.at(pointer)
                      << Term::style(Term::Style::RESET)
                      /***********/

                      /* TEXT AFTER CHAR */
                      << Term::color_fg(AFTERCHARCOLOR) 
                      << text.substr(pointer + 1, round(term_size.columns / 2))
                      /*******************/

                      << repeat_str(" ", round(term_size.columns / 2) - (text.size() - (pointer))) // space after text

                      // reset color and flush
                      << Term::style(Term::Style::RESET) << std::flush;
        }

        void display_status() {
            std::lock_guard<std::mutex> lock(display_mutex); // lock mutex
            
            Cursor::set(term_size.rows - 2, 0);
            std::cout << "WORD_COUNT: " << word_count << " WORDS_DONE: " << words_done;
            Cursor::next_line();
            std::cout << "SPOS: " << spos << " EPOS: " << epos << " POINTER: " << pointer;
        }

        //void display_wpm() {
        //    std::lock_guard<std::mutex> lock(display_mutex); // lock mutex
        
        //    Cursor::set(round(term_size.rows / 2) - 4, 0);
        //    std::cout << "WPM: " << wpm;
        //}

    private:
        Screen::Size term_size = Screen::size();

        std::string text;
        int word_count = 0;
        int words_done = 0;
        int pointer = 0;

        // used for trimming
        int spos = 0;
        int epos = round(term_size.columns / 2);

        // used for real time WPM tracking
        //double wpm = 0.0;
        //std::chrono::time_point<std::chrono::steady_clock> last_word_time;
    
        // mutex to prevent concurrent access to std::cout
        std::mutex display_mutex;
};

int main() {
    RawModeGuard raw_mode_guard;
    Cursor::hide();
    Screen::clear();

    fill_screen(' ', BGFILLCOLOR);

    std::cout << Term::style(Term::Style::BOLD) << "TerType " << Term::style(Term::Style::RESET) << "- V" << VERSION;

    std::string text = "Hello, World! This is a TypeRacer clone in the terminal! This isn't the most fancy thing ever. It's just designed to show off the stuff you can do using tty-cpp. Try it out sometime!";
    Typer typer(text);
    
    Key key = Key::NUL;

    // start timer thread
    std::thread timer([&]() {
        while (true) {
            typer.display_status();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    do {
        typer.handle_key(key);
        typer.display();
    } while ((key = Term::getkey()) != Key::CTRL_C);

    safe_exit();

    return 0;
};
