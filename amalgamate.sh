#!/bin/bash
# Uses Heady to generate a single header file
# :WARNING: ASSUMES A HEADY BINARY NAMED "heady" IS IN THE ROOT DIRECTORY! :WARNING:

if !( ./heady --source src --inline "heady_t" --output "include/tty-cpp.hpp" ) ; then
    echo "Heady failed, please make sure a compiled Heady binary is in your current directory!"
fi

