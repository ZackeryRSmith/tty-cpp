#include "../include/tty-cpp.hpp"
#include <cstdlib>
#include <iostream>

int main() {
    try {
    // tty-cpp version and link to the repo
    std::cout << Term::style(Term::Style::BOLD) << "Running tty-cpp version: " << Term::style(Term::Style::RESET) << Term::Version 
              << Term::style(Term::Style::BOLD) << "\nRepo: " << Term::style(Term::Style::RESET) << Term::Repo 
              << std::endl << std::endl;

    if (Term::stdout_connected()) 
        std::cout << Term::color_fg(Term::ColorBit4::GREEN) << "Standard output is attached to a terminal." << std::endl << std::endl;
    else
        std::cout << Term::color_fg(Term::ColorBit4::RED) << "Standard output is not attached to a terminal." << std::endl << std::endl;

    std::string text = "Some text with " + color_fg(Term::ColorBit4::RED) + color_bg(Term::ColorBit4::GREEN) + "red on green" + color_bg(Term::ColorBit4::DEFAULT) + color_fg(Term::ColorBit4::DEFAULT);
    text += " and some " + style(Term::Style::BOLD) + "bold text" + style(Term::Style::RESET) + ".\n";
    text += "𝓤𝓷𝓲𝓬𝓸𝓭𝓮 𝔀𝓸𝓻𝓴𝓼 𝓽𝓸𝓸";
    std::cout << text << std::endl;

    std::string rgb_text = "Some Text in " + Term::color_fg(255, 0, 0) + 'R' + Term::color_fg(0, 255, 0) + 'G' + Term::color_fg(0, 0, 255) + 'B' + Term::color_fg(Term::ColorBit4::DEFAULT);
    std::cout << rgb_text << std::endl;

    std::cout << "A color chart: \n";
    for (std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(i, 0, 0) << " " << Term::color_bg(Term::ColorBit4::DEFAULT); }
    std::cout << "\n";
    
    for (std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(0, i, 0) << " " << Term::color_bg(Term::ColorBit4::DEFAULT); }
    std::cout << "\n";
    
    for (std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(0, 0, i) << " " << Term::color_bg(Term::ColorBit4::DEFAULT); }
    std::cout << "\n";
    
    for (std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(i, i, i) << " " << Term::color_bg(Term::ColorBit4::DEFAULT); }
    std::cout << "\n";

    std::cout << "\nColor conversion (4bit base):\n";
    std::cout << "4bit original: " << Term::color_bg(Term::ColorBit4::BLACK) << " " << Term::color_bg(Term::ColorBit4::RED) << " " << Term::color_bg(Term::ColorBit4::GREEN) << " " << Term::color_bg(Term::ColorBit4::YELLOW) << " " << Term::color_bg(Term::ColorBit4::BLUE)
              << " " << Term::color_bg(Term::ColorBit4::MAGENTA) << " " << Term::color_bg(Term::ColorBit4::CYAN) << " " << Term::color_bg(Term::ColorBit4::WHITE) << " " << Term::color_bg(Term::ColorBit4::GRAY) << " " << Term::color_bg(Term::ColorBit4::RED_BRIGHT) << " "
              << Term::color_bg(Term::ColorBit4::GREEN_BRIGHT) << " " << Term::color_bg(Term::ColorBit4::YELLOW_BRIGHT) << " " << Term::color_bg(Term::ColorBit4::BLUE_BRIGHT) << " " << Term::color_bg(Term::ColorBit4::MAGENTA_BRIGHT) << " "
              << Term::color_bg(Term::ColorBit4::CYAN_BRIGHT) << " " << Term::color_bg(Term::ColorBit4::WHITE_BRIGHT) << " " << Term::color_bg(Term::ColorBit4::DEFAULT) << "\n";
    std::cout << "4bit to 4bit:  " << Term::color_bg(Term::rgb_to_bit4(Term::bit4_to_rgb(Term::ColorBit4::BLACK))) << " " << Term::color_bg(Term::rgb_to_bit4(Term::bit4_to_rgb(Term::ColorBit4::RED))) << " "
              << Term::color_bg(Term::rgb_to_bit4(Term::bit4_to_rgb(Term::ColorBit4::GREEN))) << " " << Term::color_bg(Term::rgb_to_bit4(Term::bit4_to_rgb(Term::ColorBit4::YELLOW))) << " " << Term::color_bg(Term::rgb_to_bit4(Term::bit4_to_rgb(Term::ColorBit4::BLUE)))
              << " " << Term::color_bg(Term::rgb_to_bit4(Term::bit4_to_rgb(Term::ColorBit4::MAGENTA))) << " " << Term::color_bg(Term::rgb_to_bit4(Term::bit4_to_rgb(Term::ColorBit4::CYAN))) << " "
              << Term::color_bg(Term::rgb_to_bit4(Term::bit4_to_rgb(Term::ColorBit4::WHITE))) << " " << Term::color_bg(Term::rgb_to_bit4(Term::bit4_to_rgb(Term::ColorBit4::GRAY))) << " "
              << Term::color_bg(Term::rgb_to_bit4(Term::bit4_to_rgb(Term::ColorBit4::RED_BRIGHT))) << " " << Term::color_bg(Term::rgb_to_bit4(Term::bit4_to_rgb(Term::ColorBit4::GREEN_BRIGHT))) << " "
              << Term::color_bg(Term::rgb_to_bit4(Term::bit4_to_rgb(Term::ColorBit4::YELLOW_BRIGHT))) << " " << Term::color_bg(Term::rgb_to_bit4(Term::bit4_to_rgb(Term::ColorBit4::BLUE_BRIGHT))) << " "
              << Term::color_bg(Term::rgb_to_bit4(Term::bit4_to_rgb(Term::ColorBit4::MAGENTA_BRIGHT))) << " " << Term::color_bg(Term::rgb_to_bit4(Term::bit4_to_rgb(Term::ColorBit4::CYAN_BRIGHT))) << " "
              << Term::color_bg(Term::rgb_to_bit4(Term::bit4_to_rgb(Term::ColorBit4::WHITE_BRIGHT))) << " " << Term::color_bg(Term::ColorBit4::DEFAULT) << "\n";

    std::cout << "4bit to 24bit: " << Term::color_bg(Term::bit4_to_rgb(Term::ColorBit4::BLACK)) << " " << Term::color_bg(Term::bit4_to_rgb(Term::ColorBit4::RED)) << " " << Term::color_bg(Term::bit4_to_rgb(Term::ColorBit4::GREEN)) << " "
              << Term::color_bg(Term::bit4_to_rgb(Term::ColorBit4::YELLOW)) << " " << Term::color_bg(Term::bit4_to_rgb(Term::ColorBit4::BLUE)) << " " << Term::color_bg(Term::bit4_to_rgb(Term::ColorBit4::MAGENTA)) << " "
              << Term::color_bg(Term::bit4_to_rgb(Term::ColorBit4::CYAN)) << " " << Term::color_bg(Term::bit4_to_rgb(Term::ColorBit4::WHITE)) << " " << Term::color_bg(Term::bit4_to_rgb(Term::ColorBit4::GRAY)) << " "
              << Term::color_bg(Term::bit4_to_rgb(Term::ColorBit4::RED_BRIGHT)) << " " << Term::color_bg(Term::bit4_to_rgb(Term::ColorBit4::GREEN_BRIGHT)) << " " << Term::color_bg(Term::bit4_to_rgb(Term::ColorBit4::YELLOW_BRIGHT)) << " "
              << Term::color_bg(Term::bit4_to_rgb(Term::ColorBit4::BLUE_BRIGHT)) << " " << Term::color_bg(Term::bit4_to_rgb(Term::ColorBit4::MAGENTA_BRIGHT)) << " " << Term::color_bg(Term::bit4_to_rgb(Term::ColorBit4::CYAN_BRIGHT)) << " "
              << Term::color_bg(Term::bit4_to_rgb(Term::ColorBit4::WHITE_BRIGHT)) << " " << Term::color_bg(Term::ColorBit4::DEFAULT) << "\n";

    std::cout << "4bit to 8bit:  " << Term::color_bg(Term::rgb_to_bit8(Term::bit4_to_rgb(Term::ColorBit4::BLACK))) + " " << Term::color_bg(Term::rgb_to_bit8(Term::bit4_to_rgb(Term::ColorBit4::RED))) + " "
              << Term::color_bg(Term::rgb_to_bit8(Term::bit4_to_rgb(Term::ColorBit4::GREEN))) + " " << Term::color_bg(Term::rgb_to_bit8(Term::bit4_to_rgb(Term::ColorBit4::YELLOW))) + " "
              << Term::color_bg(Term::rgb_to_bit8(Term::bit4_to_rgb(Term::ColorBit4::BLUE))) + " " << Term::color_bg(Term::rgb_to_bit8(Term::bit4_to_rgb(Term::ColorBit4::MAGENTA))) + " "
              << Term::color_bg(Term::rgb_to_bit8(Term::bit4_to_rgb(Term::ColorBit4::CYAN))) + " " << Term::color_bg(Term::rgb_to_bit8(Term::bit4_to_rgb(Term::ColorBit4::WHITE))) + " " << Term::color_bg(Term::rgb_to_bit8(Term::bit4_to_rgb(Term::ColorBit4::GRAY))) + " "
              << Term::color_bg(Term::rgb_to_bit8(Term::bit4_to_rgb(Term::ColorBit4::RED_BRIGHT))) + " " << Term::color_bg(Term::rgb_to_bit8(Term::bit4_to_rgb(Term::ColorBit4::GREEN_BRIGHT))) + " "
              << Term::color_bg(Term::rgb_to_bit8(Term::bit4_to_rgb(Term::ColorBit4::YELLOW_BRIGHT))) + " " << Term::color_bg(Term::rgb_to_bit8(Term::bit4_to_rgb(Term::ColorBit4::BLUE_BRIGHT))) + " "
              << Term::color_bg(Term::rgb_to_bit8(Term::bit4_to_rgb(Term::ColorBit4::MAGENTA_BRIGHT))) + " " << Term::color_bg(Term::rgb_to_bit8(Term::bit4_to_rgb(Term::ColorBit4::CYAN_BRIGHT))) + " "
              << Term::color_bg(Term::rgb_to_bit8(Term::bit4_to_rgb(Term::ColorBit4::WHITE_BRIGHT))) + " " << Term::color_bg(Term::ColorBit4::DEFAULT) << "\n";
    

    std::cout << "\nColor conversion (24bit)\n";
    
    /* red color space */
    std::cout << "24bit original: ";
    for (std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(i, 0, 0) << " " << Term::color_bg(Term::ColorBit4::DEFAULT); }
    std::cout << "\n24bit to 8bit:  ";
    for (std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(Term::rgb_to_bit8(Term::bit24_to_rgb(i, 0, 0))) << " " << Term::color_bg(Term::ColorBit4::DEFAULT); }
    std::cout << "\n24bit to 4bit:  ";
    for (std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(Term::rgb_to_bit4(Term::bit24_to_rgb(i, 0, 0))) << " " << Term::color_bg(Term::ColorBit4::DEFAULT); }
    
    /* green color space */
    std::cout << "\n24bit original: ";
    for (std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(0, i, 0) << " " << Term::color_bg(Term::ColorBit4::DEFAULT); }
    std::cout << "\n24bit to 8bit:  ";
    for (std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(Term::rgb_to_bit8(Term::bit24_to_rgb(0, i, 0))) << " " << Term::color_bg(Term::ColorBit4::DEFAULT); }
    std::cout << "\n24bit to 4bit:  ";
    for (std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(Term::rgb_to_bit4(Term::bit24_to_rgb(0, i, 0))) << " " << Term::color_bg(Term::ColorBit4::DEFAULT); }
    
    /* blue color space */
    std::cout << "\n24bit original: ";
    for (std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(0, 0, i) << " " << Term::color_bg(Term::ColorBit4::DEFAULT); }
    std::cout << "\n24bit to 8bit:  ";
    for (std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(Term::rgb_to_bit8(Term::bit24_to_rgb(0, 0, i))) << " " << Term::color_bg(Term::ColorBit4::DEFAULT); }
    std::cout << "\n24bit to 4bit:  ";
    for (std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(Term::rgb_to_bit4(Term::bit24_to_rgb(0, 0, i))) << " " << Term::color_bg(Term::ColorBit4::DEFAULT); }
    
    /* black / grey color space */
    std::cout << "\n24bit original: ";
    for (std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(i, i, i) << " " << Term::color_bg(Term::ColorBit4::DEFAULT); }
    std::cout << "\n24bit to 8bit:  ";
    for (std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(Term::rgb_to_bit8(Term::bit24_to_rgb(i, i, i))) << " " << Term::color_bg(Term::ColorBit4::DEFAULT); }
    std::cout << "\n24bit to 4bit:  ";
    for (std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(Term::rgb_to_bit4(Term::bit24_to_rgb(i, i, i))) << " " << Term::color_bg(Term::ColorBit4::DEFAULT); }
    std::cout << "\n";
    
    } catch (const Term::Exception& re) {
        std::cerr << "tty-cpp error: " << re.what() << std::endl;
        return 2;
    } catch (...) {
        std::cerr << "Unknown error." << std::endl;
        return EXIT_FAILURE;
    }
}
