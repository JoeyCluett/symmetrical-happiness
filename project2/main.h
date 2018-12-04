#pragma once

#include <iostream>
#include <stdexcept>

#include <instruction_queue.h>
#include <register_file.h>
#include <reservation_station.h>
#include <functional_unit.h>
#include <reorder_buffer.h>

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

    const int state_prep_functional_units      = 0;
    const int state_instruction_issue          = 1;
    const int state_instruction_dispatch       = 2;
    const int state_clean_reservation_stations = 3; // issued rs can dispatch on the next cycle
    int current_state = state_instruction_issue;

    bool cycle_finished = false;
    while(!cycle_finished) {
        switch(current_state) {
            case state_prep_functional_units:
                {
                    for(int i : {0, 1})
                        fu[i].advance_clock_pointer();

                    int active_fu = find_broadcastable_functional_unit(fu);
                    if(active_fu >= 0) {
                        std::cout << "Ready functional unit: " << active_fu << std::endl;
                        functional_unit_t& fu_ref = fu[active_fu];

                        auto bcast_to_rs = [](reservation_station_t& rs, int rs_tag, int value) {
                            if(rs.Qk == rs_tag) {
                                rs.Qk = -1;
                                rs.Vk = value;
                                rs.flag_was_broadcast = true;
                            }

                            if(rs.Qj == rs_tag) {
                                rs.Qj = -1;
                                rs.Vj = value;
                                rs.flag_was_broadcast = true;
                            }
                        };

                        // broadcast this result to all reservation stations waiting for it
                        for(int i : {0, 1, 2, 3, 4}) {
                            bcast_to_rs(rs[i], fu_ref.from_reservation_station, fu_ref.get_value());
                        }

                        fu[active_fu].reset();
                    }
                }
                current_state = state_instruction_issue;
                break;
            case state_instruction_issue:
                {
                    instruction_entry_t ie;
                    if(iq.next_instruction(ie)) {
                        switch(ie.op) {
                            case operation_t::_add:
                            case operation_t::_subtract:
                                for(int i : {0, 1, 2}) {
                                    if(rs[i].can_issue()) {
                                        rs[i].insert_instruction(ie, rf, i);
                                        iq.advance_program_counter();
                                        break;
                                    }
                                }
                                break;
                            case operation_t::_multiply:
                            case operation_t::_divide:
                                for(int i : {3, 4}) {
                                    if(rs[i].can_issue()) {
                                        rs[i].insert_instruction(ie, rf, i);
                                        iq.advance_program_counter();
                                        break;
                                    }
                                }
                                break;
                            default:
                                throw std::runtime_error("trying to issue unknown instruction");
                        }
                    }
                }
                current_state = state_instruction_dispatch;
                break;
            case state_instruction_dispatch:
                {
                    // search through functional units and find 
                    // an instruction to dispatch if possible
                    if(fu[0].current_operation == operation_t::_none) {   
                        for(int i : {0, 1, 2}) {
                            if(rs[i].can_dispatch()) {
                                fu[0].clocks_to_finish = clocks_for_operation(rs[i].current_operation);
                                fu[0].current_clocks = 0;
                                fu[0].dest_reg = rs[i].dest_register;
                                fu[0].operand_1 = rs[i].Vj;
                                fu[0].operand_2 = rs[i].Vk;
                                fu[0].current_operation = rs[i].current_operation;

                                rs[i].busy = true;

                                break;
                            }
                        }
                    }

                    // search for multiply/divide unit
                    if(fu[1].current_operation == operation_t::_none) {
                        for(int i : {3, 4}) {
                            if(rs[i].can_dispatch()) {
                                fu[1].clocks_to_finish = clocks_for_operation(rs[i].current_operation);
                                fu[1].current_clocks = 0;
                                fu[1].dest_reg = rs[i].dest_register;
                                fu[1].operand_1 = rs[i].Vj;
                                fu[1].operand_2 = rs[i].Vk;
                                fu[1].current_operation = rs[i].current_operation;

                                rs[i].busy = true;

                                break;
                            }
                        }
                    }
                }
                current_state = state_clean_reservation_stations;
                break;
            case state_clean_reservation_stations:

                // prep reservation stations for next cycle
                for(int i : {0, 1, 2, 3, 4}) {
                    rs[i].flag_was_issued = false;
                    rs[i].flag_was_broadcast = false;
                }

                cycle_finished = true;
                break;
            default:
                throw std::runtime_error("ERROR");
        }
    }
}
