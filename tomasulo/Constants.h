#ifndef __JJC__CONSTANTS__H__
#define __JJC__CONSTANTS__H__
/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

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