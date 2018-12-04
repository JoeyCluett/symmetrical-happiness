#pragma once

#include <vector>
#include <iostream>

enum class operation_t : int {
    _add = 0, 
    _subtract = 1, 
    _multiply = 2, 
    _divide = 3,
    _none = 4 // station is free
};

void print_operation(operation_t op) {
    switch(op) {
        case operation_t::_add:
            std::cout << "OPERATION: add\n"; break;
        case operation_t::_subtract:
            std::cout << "OPERATION: subtract\n"; break;
        case operation_t::_multiply:
            std::cout << "OPERATION: multiply\n"; break;
        case operation_t::_divide:
            std::cout << "OPERATION: divide\n"; break;
        case operation_t::_none:
            std::cout << "OPERATION: none\n"; break;
    }
}

class instruction_entry_t {
public:
    operation_t op = operation_t::_none;
    int dest = -1, src1 = 0, src2 = 0;

    int program_counter = -1;
};

class instruction_queue_t {
public:
    // a sort of program counter to track progress
    int current_instruction = 0;

    // instructions are added here
    std::vector<instruction_entry_t> stream;
    
    friend std::ostream& operator<<(std::ostream& os, instruction_queue_t& iq) {
        os << "+---------------------\n";
        os << "| Instruction queue\n";
        os << "+---------------------\n";
        for(auto& ie : iq.stream) {
            os << "| R[" << ie.dest << "] = R[" << ie.src1 << "] ";

            switch(ie.op) {
                case operation_t::_add:
                    os << "+ R["; break;
                case operation_t::_subtract:
                    os << "- R["; break;
                case operation_t::_multiply:
                    os << "* R["; break;
                case operation_t::_divide:
                    os << "/ R["; break;
                default:
                    break;
            }

            os << ie.src2 << "]\n";
        }
        return os;
    }

    // returns false if there are no more instructions
    bool next_instruction(instruction_entry_t& ie) {
        if(current_instruction >= stream.size())
            return false;
        ie = stream[current_instruction];
        ie.program_counter = current_instruction;
        return true;
    }

    void advance_program_counter(void) {
        current_instruction++;
    }
};