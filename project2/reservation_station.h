#pragma once

#include <vector>
#include <instruction_queue.h>
#include <register_file.h>
#include <functional_unit.h>

class reservation_station_t {
public:
    reservation_station_t(void) { ; }

    operation_t current_operation = operation_t::_none; // none = not busy
    bool busy = false;

    int 
        Vj = 0, 
        Vk = 0, 
        Qj = -1, 
        Qk = -1; // -1 : NONE

    int dest_register = -1;

    // flags used by simulator to track simulation events
    bool flag_has_dispatch = false;
    bool flag_has_issue = false;
    bool flag_was_broadcast_to = false;

    void reset(void) {
        this->Vj = 0;
        this->Vk = 0;
        this->Qj = -1;
        this->Qk = -1;
        this->dest_register = -1;
        this->busy = false;
        this->current_operation = operation_t::_none;
    }

    bool can_dispatch(void) {
        if(this->current_operation != operation_t::_none) {
            // MIGHT be able to dispatch
            if(this->flag_has_issue || this->busy)
                return false; // cant issue and dispatch in same cycle or if already doing something
            else {
                if(this->Qj == -1 && this->Qk == -1)
                    return true;
                else
                    return false;
            }
        } else {
            return false;
        }
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
        this->flag_has_issue = true;
        //this->busy = true;

        // change RF tag to this station
        rf.tag(ie.dest) = rs_index;
        this->dest_register = ie.dest;
        this->current_operation = ie.op;

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

            if(tag == -1) {
                this->Vk = value;
                this->Qk = -1;
            } else {
                this->Qk = tag;
            }
        }


    }
};