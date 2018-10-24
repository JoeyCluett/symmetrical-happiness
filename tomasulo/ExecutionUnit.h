#ifndef __JJC__EXECUTION__UNIT__H__
#define __JJC__EXECUTION__UNIT__H__

#include <iostream>
#include <stdexcept>
#include "RegisterFile.h"

// could just use structs at this point
class ExecutionUnit {
public:
    int busy = false;
    
    int operation;
    int cycles_to_complete;

    // increment every clock cycle
    int current_cycles = 0;

    MultiType output = NONE;
    MultiType input_1;
    MultiType input_2;

    friend std::ostream& operator<<(std::ostream& os, ExecutionUnit& eu) {
        switch(eu.operation) {
            case ADD:
                os << eu.output.i << " = " << eu.input_1.i << " + " << eu.input_2.i << std::endl;
                break;
            case SUB:
                os << eu.output.i << " = " << eu.input_1.i << " - " << eu.input_2.i << std::endl;
                break;
            case MUL:
                os << eu.output.i << " = " << eu.input_1.i << " * " << eu.input_2.i << std::endl;
                break;
            case DIV:
                os << eu.output.i << " = " << eu.input_1.i << " / " << eu.input_2.i << std::endl;
                break;
            case NONE:
                os << "NO OP\n";
                break;
            default:
                throw std::runtime_error("ExecutionUnit::operator<<() -> undefined operation");
                break;
        }

        return os;
    }

};

#endif // __JJC__EXECUTION__UNIT__H__