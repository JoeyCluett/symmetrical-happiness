#ifndef __JJC__EXECUTION__UNIT__H__
#define __JJC__EXECUTION__UNIT__H__

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