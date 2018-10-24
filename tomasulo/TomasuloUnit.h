#ifndef __JJC__TOMASULO__UNIT__H__
#define __JJC__TOMASULO__UNIT__H__

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

public:
    TomasuloUnit(
            std::initializer_list<ReservationStationGroup*> res_groups,
            InstructionQueue& iq_ref,
            RegisterFile& rf) {
        
        for(auto rsg : res_groups)
            this->res_stations.push_back(rsg);
        this->iq = &iq_ref;
        this->rf = &rf;
    }

    friend std::ostream& operator<<(std::ostream& os, TomasuloUnit& tu) {
        os << *tu.iq << std::endl;

        for(auto ptr : tu.res_stations) {
            os << *ptr << std::endl;
        }

        os << *tu.rf << std::endl;

        return os;
    }

    void simulate(int cycles) {
        const int STATE_issue     = 0;
        const int STATE_dispatch  = 1;
        const int STATE_broadcast = 2;
        int current_state = STATE_issue;

        for(int current_cycle = 0; current_cycle < cycles; /* incremented by FSM */) {
            switch(current_state) {
                case STATE_issue:
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
                                    //std::cout << "    issuing instruction\n" << std::flush;
                                    ptr->issue(inst, rf);
                                    //std::cout << "    advancing instruction pointer\n" << std::flush;
                                    this->iq->advanceIPtr(); // instruction issue worked

                                    // register file RAT needs to point to correct RS
                                                           

                                    break;
                                }
                            }
                        }
                    }
                    current_state = STATE_dispatch;
                    break;
                case STATE_dispatch:
                    {

                    }
                    current_state = STATE_broadcast;
                    break;
                case STATE_broadcast:
                    {
                        
                    }
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