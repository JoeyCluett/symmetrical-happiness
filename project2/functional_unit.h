#pragma once

#include <iostream>
#include <instruction_queue.h>

class functional_unit_t {
public:
    // starts out empty
    operation_t current_operation = operation_t::_none;

    int operand_1 = 0, operand_2 = 0;
    int dest_reg = -1;

    // different for each functional unit
    int current_clocks = 0;
    int clocks_to_finish = 0;

    // needed for broadcast stage
    int from_reservation_station = -1;

    bool flag_has_exception = false;

    int get_value(void) {
        switch(this->current_operation) {
            case operation_t::_add:
                return operand_1 + operand_2; break;
            case operation_t::_subtract:
                return operand_1 - operand_2; break;
            case operation_t::_multiply:
                return operand_1 * operand_2; break;
            case operation_t::_divide:
                return operand_1 / operand_2; break;
            default:
                throw std::runtime_error("funtional_unit_t::get_value -> unknown operation");
        }
    }

    bool ready_to_broadcast(void) {
        return (current_clocks == clocks_to_finish && this->current_operation != operation_t::_none);
    }

    bool advance_clock_pointer(void) {
        if(
                this->current_operation != operation_t::_none && 
                this->current_clocks < this->clocks_to_finish)
            this->current_clocks++;
        return (current_clocks == clocks_to_finish);
    }

    friend std::ostream& operator<<(std::ostream& os, functional_unit_t& fu) {
        os << "+-----------------\n";
        os << "| ";

        switch(fu.current_operation) {
            case operation_t::_add:
                os << "add       " << fu.current_clocks << '/' << fu.clocks_to_finish << "\n"; break;
            case operation_t::_subtract:
                os << "subtract  " << fu.current_clocks << '/' << fu.clocks_to_finish << "\n"; break;
            case operation_t::_multiply:
                os << "multiply  " << fu.current_clocks << '/' << fu.clocks_to_finish << "\n"; break;
            case operation_t::_divide:
                os << "divide    " << fu.current_clocks << '/' << fu.clocks_to_finish << "\n"; break;
            case operation_t::_none:
                os << "none      ""\n"; break;
            default:
                os << "UNKNOWN\n";
                break;
        }

        switch(fu.current_operation) {
            case operation_t::_add:
                os << "| R[" << fu.dest_reg << "] = " << fu.operand_1 << " + " << fu.operand_2 << "\n";
                break;
            case operation_t::_subtract:
                os << "| R[" << fu.dest_reg << "] = " << fu.operand_1 << " - " << fu.operand_2 << "\n";
                break;
            case operation_t::_multiply:
                os << "| R[" << fu.dest_reg << "] = " << fu.operand_1 << " * " << fu.operand_2 << "\n";
                break;
            case operation_t::_divide:
                os << "| R[" << fu.dest_reg << "] = " << fu.operand_1 << " / " << fu.operand_2 << "\n";
                break;
            default:
                os << "| no operation\n";
        }

        os << "+-----------------\n";
        return os;
    }

};