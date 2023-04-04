#pragma once

enum class Key {
    UNKNOWN = -1,
    // ASCII values for control characters
    NUL = 0,  // Null character
    SOH = 1,  // Start of Heading
    STX = 2,  // Start of Text
    ETX = 3,  // End of Text
    EOT = 4,  // End of Transmission
    ENQ = 5,  // Enquiry
    ACK = 6,  // Acknowledge
    BEL = 7,  // Bell, Alert
    BS = 8,   // Backspace
    HT = 9,   // Horizontal Tab
    LF = 10,  // Line Feed
    VT = 11,  // Vertical Tabulation
    FF = 12,  // Form Feed
    CR = 13,  // Carriage Return
    SO = 14,  // Shift Out
    SI = 15,  // Shift In
    DLE = 16, // Data Link Escape
    DC1 = 17, // Device Control One (XON)
    DC2 = 18, // Device Control Two
    DC3 = 19, // Device Control Three (XOFF)
    DC4 = 20, // Device Control Four
    NAK = 21, // Negative Acknowledge
    SYN = 22, // Synchronous Idle
    ETB = 23, // End of Transmission Block
    CAN = 24, // Cancel
    EM = 25,  // End of Medium
    SUB = 26, // Substitute
    ESC = 27, // Escape
    FS = 28,  // File Separator
    GS = 29,  // Group Separator 
    RS = 30,  // Record Separator
    US = 31,  // Unit Separator
    // Regular characters (ASCII values 32 to 126)
    // :DEPRECTED: TTY-CPP supports comparing against a char
    SPACE = 32,
    //EXCLAMATION_MARK = 33,
    //DOUBLE_QUOTES = 34,
    //NUMBER_SIGN = 35,
    //DOLLAR = 36,
    //PERCENT_SIGN = 37,
    //AMPERSAND = 38,
    //SINGLE_QUOTE = 39,
    //OPEN_BRACKET = 40,
    //CLOSE_BRACKET = 41,
    //ASTERISK = 42,
    //PLUS = 43,
    //COMMA = 44,
    //HYPHEN = 45,
    //PERIOD = 46,
    //SLASH = 47,
    //ZERO = 48,
    //ONE = 49,
    //TWO = 50,
    //THREE = 51,
    //FOUR = 52,
    //FIVE = 53,
    //SIX = 54,
    //SEVEN = 55,
    //EIGHT = 56,
    //NINE = 57,
    //TILDE = 126,
    // ...
    DEL = 127,
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
    // Add ASCII characters directly
    A = 'A',
    B = 'B',
    C = 'C',
    D = 'D',
    E = 'E',
    F = 'F',
    G = 'G',
    H = 'H',
    I = 'I',
    J = 'J',
    K = 'K',
    L = 'L',
    M = 'M',
    N = 'N',
    O = 'O',
    P = 'P',
    Q = 'Q',
    R = 'R',
    S = 'S',
    T = 'T',
    U = 'U',
    V = 'V',
    W = 'W',
    X = 'X',
    Y = 'Y',
    Z = 'Z',
    a = 'a',
    b = 'b',
    c = 'c',
    d = 'd',
    e = 'e',
    f = 'f',
    g = 'g',
    h = 'h',
    i = 'i',
    j = 'j',
    k = 'k',
    l = 'l',
    m = 'm',
    n = 'n',
    o = 'o',
    p = 'p',
    q = 'q',
    r = 'r',
    s = 's',
    t = 't',
    u = 'u',
    v = 'v',
    w = 'w',
    x = 'x',
    y = 'y',
    z = 'z',
    // Special keys
    ENTER = 1000,
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

namespace Term {
Key getkey();
int keyhit();
} // namespace Term
