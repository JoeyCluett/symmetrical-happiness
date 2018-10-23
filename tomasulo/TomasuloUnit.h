#ifndef __JJC__TOMASULO__UNIT__H__
#define __JJC__TOMASULO__UNIT__H__

#include <initializer_list>
#include <vector>
#include <stdexcept>

// custom libs
#include "ReservationStation.h"
#include "InstructionQueue.h"
#include "Constants.h"
#include "RegisterFile.h"

class TomasuloUnit {
private:
    std::vector<ReservationStationGroup*> res_stations;
    InstructionQueue* iq;

public:
    TomasuloUnit(
            std::initializer_list<ReservationStationGroup*> res_groups,
            InstructionQueue& iq_ref) {
        
        for(auto rsg : res_groups)
            this->res_stations.push_back(rsg);
        this->iq = &iq_ref;
    }

    friend std::ostream& operator<<(std::ostream& os, TomasuloUnit& tu) {
        os << *tu.iq << std::endl;

        for(auto ptr : tu.res_stations) {
            os << *ptr << std::endl;
        }

        return os;
    }

    void simulate(int cycles) {
        const int STATE_issue     = 0;
        const int STATE_dispatch  = 1;
        const int STATE_broadcast = 2;

        int current_state = STATE_issue;

        for(int current_cycle = 0; current_cycle < cycles;) {
            switch(current_state) {
                case STATE_issue:
                    if(this->iq->hasNextInstruction() == true) {
                        auto inst = this->iq->getNextInstruction();

                        for(auto ptr : this->res_stations) {
                            int free_station = ptr->freeStation();
                            if(free_station >= 0) {

                                ptr->issue(inst);
                                this->iq->advanceIPtr(); // instruction issue worked
                                break;
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