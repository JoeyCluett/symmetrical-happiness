#pragma once

#include <vector>
#include <instruction_queue.h>
#include <register_file.h>
#include <functional_unit.h>

class reservation_station_t {
public:
    reservation_station_t(void) { ; }

    operation_t current_operation = operation_t::_none; // none = not occupied
    bool busy = false; // currently executing?

    int 
        Vj = 0,  // value of this operand
        Vk = 0,  // --
        Qj = -1, // reservation station with this operand
        Qk = -1; // --

    int dest_register = -1;
    int instruction_pc = -1;

    bool flag_was_issued = false;
    bool flag_was_broadcast = false;

    void reset(void) {
        this->Vj = 0;
        this->Vk = 0;
        this->Qj = -1;
        this->Qk = -1;
        this->dest_register = -1;
        this->busy = false;
        this->current_operation = operation_t::_none;
        this->instruction_pc = -1;
    }

    bool can_dispatch(void) {
        if(
                this->current_operation != operation_t::_none && 
                this->busy == false && 
                this->flag_was_issued == false &&
                this->flag_was_broadcast == false) {
            return (this->Qj == -1 && this->Qk == -1);
        } else {
            return false;
        }
    }

    bool can_issue(void) {
        return (this->current_operation == operation_t::_none);
    }

    bool can_broadcast(int rs_index) {
        if(
                this->flag_was_issued == false && 
                this->busy == false && 
                this->current_operation != operation_t::_none) {
            return true;
        }
        return false;
    }

    void dispatch_to(functional_unit_t& fu) {
        this->busy = true;
        fu.current_operation = this->current_operation;
        fu.operand_1 = this->Vj;
        fu.operand_2 = this->Vk;
        fu.dest_reg = this->dest_register;
    }

    // it has already been determined if this station is free
    void insert_instruction(instruction_entry_t& ie, register_file_t& rf, int rs_index) {

        // find data for source j
        {
            int tag = rf.tag(ie.src1);
            int value = rf.value(ie.src1);

            if(tag == -1) {
                // value is immediately available
                this->Vj = value;
                this->Qj = -1;
            } else {
                this->Qj = tag;
            }
        }

        // find data for source k
        {
            int tag = rf.tag(ie.src2);
            int value = rf.value(ie.src2);

            std::cout << "K tag: " << tag << std::endl;

            if(tag == -1) {
                this->Vk = value;
                this->Qk = -1;
            } else {
                this->Qk = tag;
            }
        }

        // change RF tag to this station
        rf.tag(ie.dest) = rs_index;
        this->flag_was_issued = true;
        this->dest_register = ie.dest;
        this->current_operation = ie.op;
        this->instruction_pc = ie.program_counter;
    }
};