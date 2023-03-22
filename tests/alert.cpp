// choose fun
#include "../include/tty-cpp.hpp"
#include "dialog.hpp"
#include <iostream>
#include <string>

int main() {
    RawModeGuard raw_mode_guard;
    Screen::clear();
    Cursor::hide();
    
    std::cout << "Hello, World! How common is this phrase. Very common. We use it quite often while learning a programming language or when you are tesing something, for example I'm doing that now. Will the screen save? With the alert overlap? With the final sreen be restored? At what speed? What are the bug? These are all the kinds of questions I'm asking myself as I write this long message. All for a test jeez. Actually I can speed this up. Let's just use the Lorem ipsum.\n\nLorem ipsum dolor sit amet, consectetur adipiscing elit. Fusce vel felis augue. Cras maximus a nibh quis auctor. In porta mollis cursus. Aenean rutrum felis nec finibus sollicitudin. Suspendisse mi magna, dictum in ante in, luctus semper urna. Duis ultrices sed leo id placerat. Integer cursus, enim sit amet aliquet gravida, libero eros ultrices justo, sed aliquam felis ex vel arcu. Proin placerat sapien a nunc faucibus facilisis. Cras tincidunt euismod porta. Nulla consequat vehicula tortor, id volutpat libero aliquet id. Sed dignissim odio faucibus tortor luctus, vel imperdiet libero gravida. Suspendisse sit amet sodales ligula. Mauris placerat a orci a placerat. Duis id faucibus tellus, a convallis ante. Quisque vel est quis massa fringilla posuere at eget felis." << std::flush;


    Key key;
    while ((key = Term::getkey()) != Key::CTRL_C) {
        if (key == Key::ENTER) {
            alert("INFO", "ACKNOWLEDGE", 
                  Term::style(Term::Style::BOLD) + "Running tty-cpp version: " + Term::style(Term::Style::RESET) 
                  + Term::VERSION + '\n' +

                  Term::style(Term::Style::BOLD) + "Repo: " + Term::style(Term::Style::RESET)
                  + Term::REPO + "\n\n" +

                  "This test is quite clunky, buggy, and the code is... well less then perfect. This test was written to demonstrate\nwhat is possible using TTY-CPP. This test will be improved as TTY-CPP evolves into a more mature library.\n\n" +

                  Term::color_fg(Term::ColorBit4::GRAY) + "press enter to acknowledge" + Term::color_fg(Term::ColorBit4::DEFAULT));
            Screen::restore();
        }
    }

    Screen::clear();
    Cursor::show();
    return 0;
}
