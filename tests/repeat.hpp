// function that repeats a string n times
#pragma once

#include <string>

std::string repeatStr(const std::string& str, const size_t n) {
    std::string result;
    result.reserve(n * str.length());
    
    for (size_t i = 0; i < n; ++i) {
        result += str;
    }
    
    return result;
}
