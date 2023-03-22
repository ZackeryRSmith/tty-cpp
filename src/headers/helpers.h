#pragma once

/* guard to make sure raw mode is exited on program exit */
class RawModeGuard {
public:
    RawModeGuard() { Term::enable_raw_mode(); }
    ~RawModeGuard() { Term::disable_raw_mode(); }
};

/* scope guard which executes a function upon deconstruction */
class ScopeGuard {
public:
    ScopeGuard(std::function<void()> f) : func(f) {}
    ~ScopeGuard() { func(); }

private:
    std::function<void()> func;
};


