#ifndef __JJC__CONSTANTS__H__
#define __JJC__CONSTANTS__H__

// integer add instructions
#define ADD (0)
#define SUB (1)
#define MUL (2)
#define DIV (3)
#define NONE (4)

// plenty of space for other instruction opcodes
// ...

#include <vector>

// operation lookup table for string ops
const std::vector<const char*> operation_lut = {
    "ADD        ",
    "SUB        ",
    "MUL        ",
    "DIV        ",
    "UNASSIGNED "
};

const std::vector<const char*> op_symbol_lut = {
    "+", "-", "*", "/", "UNKNOWN"
};

#ifdef _WIN32
#   define system("cls")
#elif __LINUX__
#   define CLEAR_SCREEN system("clear")
#endif // __LINUX__

#endif // __JJC__CONSTANTS__H__