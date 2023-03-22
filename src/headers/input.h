#pragma once

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
    DEL = 32,
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
} // namespace Term
