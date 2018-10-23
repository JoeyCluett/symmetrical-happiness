#ifndef __JJC__CONSTANTS__H__
#define __JJC__CONSTANTS__H__

// integer add instructions
#define ADD (0)
#define SUB (1)
#define MUL (2)
#define DIV (3)

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

#endif // __JJC__CONSTANTS__H__