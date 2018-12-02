#pragma once

#include <iostream>
#include <stdexcept>

#include <instruction_queue.h>
#include <register_file.h>
#include <reservation_station.h>
#include <functional_unit.h>

int clocks_for_operation(operation_t op) {
    switch(op) {
        case operation_t::_add:
            return 2; break;
        case operation_t::_subtract:
            return 2; break;
        case operation_t::_multiply:
            return 10; break;
        case operation_t::_divide:
            return 40; break;
        default:
            throw std::runtime_error("clocks_for_operation -> unknown operation");
    }
}

// returns -1 on failure
int free_reservation_station(reservation_station_t* rs, operation_t op) {
    switch(op) {
        case operation_t::_add:
        case operation_t::_subtract:
            for(int i : {0, 1, 2}) {
                if(rs[i].current_operation == operation_t::_none)
                    return i;
            }
            break;
        case operation_t::_multiply:
        case operation_t::_divide:
            for(int i :{3, 4}) {
                if(rs[i].current_operation == operation_t::_none)
                    return i;
            }
            break;
        default:
            throw std::runtime_error("unsupported operation in main.h 'free_reservation_station'");
    }
    return -1; // no free station for this operation
}

int find_broadcastable_functional_unit(functional_unit_t* fu) {
    int num_broadcastable = 0;
    num_broadcastable += (int)fu[0].ready_to_broadcast();
    num_broadcastable += (int)fu[1].ready_to_broadcast();

    switch(num_broadcastable) {
        case 0:
            return -1; 
            break;
        case 1:
            if(fu[0].ready_to_broadcast()) {
                return 0;
            } else {
                return 1;
            }
            break;
        case 2:
            // most complex case
            return (fu[0].clocks_to_finish > fu[1].clocks_to_finish ? 0 : 1);
            break;
    }
}

// simualate a single clock cycle
void simulate_cycle(
        reservation_station_t* rs, 
        instruction_queue_t& iq, 
        register_file_t& rf,
        functional_unit_t* fu) {

    const int state_instruction_issue     = 0;
    const int state_instruction_dispatch  = 1;
    const int state_instruction_broadcast = 2;
    int current_state = state_instruction_issue;

    bool cycle_finished = false;
    while(!cycle_finished) {
        switch(current_state) {
            case state_instruction_issue:
                {
                    // end of cycle, reset has_issue flags so they can be dispatched next cycle
                    for(int i : {0, 1, 2, 3, 4})
                        rs[i].flag_has_issue = false;

                    // advance functional units here
                    for(int i : {0, 1})
                        fu[i].advance_clock_pointer();

                    // broadcast any result that is needed and prep functional unit for dispatch
                    int broadcastable_fu = find_broadcastable_functional_unit(fu);
                    functional_unit_t& fu_ref = fu[broadcastable_fu];

                    

                    instruction_entry_t ie;
                    auto res = iq.next_instruction(ie);

                    if(res) {
                        int free_rs = free_reservation_station(rs, ie.op);
                        if(free_rs != -1) {
                            rs[free_rs].insert_instruction(ie, rf, free_rs);
                            iq.advance_program_counter();
                        }
                    } else {
                        // no available instruction
                        //std::cout << "NO AVAILABLE INSTRUCTION\n";
                        ;
                    }
                    current_state = state_instruction_dispatch;
                }
                break;
            case state_instruction_dispatch:
                {
                    // dispatch instruction to add/subtract unit if possible
                    if(fu[0].current_operation == operation_t::_none) {
                        for(int i : {0, 1, 2}) {
                            if(rs[i].can_dispatch()) {
                                // move data into the functional unit
                                fu[0].current_operation = rs[i].current_operation;
                                fu[0].dest_reg = rs[i].dest_register;
                                fu[0].operand_1 = rs[i].Vj;
                                fu[0].operand_2 = rs[i].Vk;

                                fu[0].clocks_to_finish = clocks_for_operation(rs[i].current_operation);

                                rs[i].busy = true;

                                break;
                            }
                        }
                    }

                    // instruction for multiply/divide unit
                    if(fu[1].current_operation == operation_t::_none) {
                        for(int i : {3, 4}) {
                            if(rs[i].can_dispatch()) {
                                fu[1].current_operation = rs[i].current_operation;
                                fu[1].dest_reg = rs[i].dest_register;
                                fu[1].operand_1 = rs[i].Vj;
                                fu[1].operand_2 = rs[i].Vk;

                                fu[1].clocks_to_finish = clocks_for_operation(rs[i].current_operation);

                                rs[i].busy = true;

                                break;
                            }
                        }
                    }

                    current_state = state_instruction_broadcast;
                }
                break;
            case state_instruction_broadcast:
                
                cycle_finished = true;

                break;
            default:
                throw std::runtime_error("in main.h 'simulate_cycle' -> unknown state in FSM");
        }
    }
}
