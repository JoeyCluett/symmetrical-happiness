#ifndef __JJC__TOMASULO__UNIT__H__
#define __JJC__TOMASULO__UNIT__H__

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

#include <initializer_list>
#include <vector>
#include <stdexcept>
#include <iostream>

// custom libs
#include "ReservationStation.h"
#include "InstructionQueue.h"
#include "Constants.h"
#include "RegisterFile.h"

class TomasuloUnit {
private:
    std::vector<ReservationStationGroup*> res_stations;
    InstructionQueue* iq;
    RegisterFile* rf;

    int simulation_cycles = 0;

public:
    TomasuloUnit(
            std::initializer_list<ReservationStationGroup*> res_groups,
            InstructionQueue& iq_ref,
            RegisterFile& rf) {
        
        //for(auto rsg : res_groups)
        //    this->res_stations.push_back(rsg);
        this->res_stations = res_groups;
        this->iq = &iq_ref;
        this->rf = &rf;
    }

    friend std::ostream& operator<<(std::ostream& os, TomasuloUnit& tu) {
        os << "After " << tu.simulation_cycles << " clock cycles\n\n";

        os << *tu.iq << std::endl;

        for(auto ptr : tu.res_stations) {
            os << *ptr << std::endl;
        }

        os << *tu.rf << std::endl;

        return os;
    }

    void simulate(int cycles) {
        this->simulation_cycles += cycles;

        const int STATE_issue     = 0;
        const int STATE_dispatch  = 1;
        const int STATE_broadcast = 2;
        const int STATE_final     = 3;
        int current_state = STATE_issue;

        for(int current_cycle = 0; current_cycle < cycles; /* incremented by FSM */) {
            switch(current_state) {
                case STATE_issue:
                    // busy execution units advance one clock cycle
                    for(auto rsg : res_stations)
                        rsg->advanceExecutionUnit();

                    if(this->iq->hasNextInstruction() == true) {
                        auto inst = this->iq->getNextInstruction();

                        // find a reservation station group that supports the operation
                        for(auto ptr : this->res_stations) {
                            if(ptr->supportsOperation(inst.opcode)) {
                                // if reservation station does support, does it have a free entry?
                                int free_station = ptr->freeStation();

                                //std::cout << "TomasuloUnit::simulate() -> free reservation station: " 
                                //<< free_station << std::endl << std::flush;

                                if(free_station >= 0) {
                                    // reservation station is free AND supports this operation
                                    ptr->issue(inst, rf); // also sets RF RAT properly
                                    this->iq->advanceIPtr(); // instruction issue worked

                                    break;
                                }
                            }
                        }
                    }
                    current_state = STATE_dispatch;
                    break;
                case STATE_dispatch:
                    for(auto rs : res_stations) {
                        rs->dispatch();
                    }

                    // at this point any reservation stations that 
                    // received instructions can dispatch on the next cycle
                    for(auto ptr : rstation_entry_t::station_entries)
                        ptr->hadIssue = false;

                    current_state = STATE_broadcast;
                    break;
                case STATE_broadcast:
                    for(auto rsg : this->res_stations) {
                        auto eu_ptr = rsg->execUnit();
                        //std::cout << (eu_ptr->readyToBroadcast ? " ready to broadcast\n" : " not ready to broadcast\n");
                        if(eu_ptr->readyToBroadcast) {
                            // find all reservation stations waiting 
                            // for this result and copy result
                            for(auto rs_ptr : rstation_entry_t::station_entries) {
                                if(rs_ptr->busy) {
                                    if(rs_ptr->Qj == eu_ptr->source_rs) {
                                        rs_ptr->Qj = -1;
                                        rs_ptr->Vj = eu_ptr->getResult();
                                    }

                                    if(rs_ptr->Qk == eu_ptr->source_rs) {
                                        rs_ptr->Qk = -1;
                                        rs_ptr->Vk = eu_ptr->getResult();
                                    }
                                }
                            }

                            // if register file still has the tag for this 
                            // instruction, update the value
                            // sorry about the ridiculous pointer accesses
                            auto reg_ptr = this->rf->getRegister(eu_ptr->dest_reg);
                            if(reg_ptr->rat == eu_ptr->source_rs) {
                                reg_ptr->rf.i = eu_ptr->getResult();
                                reg_ptr->rat = -1; // not used anymore
                            }
                        }
                    }
                    current_state = STATE_final;
                    break;
                case STATE_final:
                    current_cycle++;
                    current_state = STATE_issue;
                    break;
                default:
                    throw std::runtime_error("TomasuloUnit::simulate() -> unknown internal state");
            }
        }

    }

};

#endif // __JJC__TOMASULO__UNIT__H__