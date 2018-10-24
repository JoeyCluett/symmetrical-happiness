#ifndef __JJC__EXECUTION__UNIT__H__
#define __JJC__EXECUTION__UNIT__H__

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

#include <iostream>
#include <stdexcept>
#include "RegisterFile.h"

// could just use structs at this point
class ExecutionUnit {
public:
    int busy = false;
    int readyToBroadcast = false;
    
    int operation;
    int cycles_to_complete = -1;

    // increment every clock cycle
    int current_cycles = 0;

    int source_rs = -1;
    int dest_reg = -1;
    MultiType input_1;
    MultiType input_2;

    int getResult(void) {
        switch(operation) {
            case ADD:
                return input_1.i + input_2.i; break;
            case SUB:
                return input_1.i - input_2.i; break;
            case MUL:
                return input_1.i * input_2.i; break;
            case DIV:
                return input_1.i / input_2.i; break;
            default:
                throw std::runtime_error("ExecutionUnit::getResult() -> unknown operation");
        }
    }

    friend std::ostream& operator<<(std::ostream& os, ExecutionUnit& eu) {
        if(eu.operation != NONE) {
            os << "R[" << eu.dest_reg << "] = " << eu.input_1.i;
            switch(eu.operation) {
                case ADD:
                    os << " + "; break;
                case SUB:
                    os << " - "; break;
                case MUL:
                    os << " * "; break;
                case DIV:
                    os << " / "; break;
                default:
                    throw std::runtime_error("ExecutionUnit::operator<<() -> undefined operation");
                    break;
            }
            os << eu.input_2.i << ", cycles[" << eu.current_cycles << '/' << eu.cycles_to_complete << "]";

            if(eu.readyToBroadcast)
                os << " READY";
            os << std::endl;
        } else {
            os << "NO_OP\n";
        }

        return os;
    }

};

#endif // __JJC__EXECUTION__UNIT__H__