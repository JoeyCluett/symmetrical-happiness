#ifndef __JJC__TOMASULO__UNIT__H__
#define __JJC__TOMASULO__UNIT__H__

#include <initializer_list>
#include <vector>

// custom libs
#include "ReservationStation.h"
#include "InstructionQueue.h"
#include "Constants.h"
#include "RegisterFile.h"

class TomasuloUnit {
private:
    std::vector<ReservationStationGroup*> res_stations;

public:
    TomasuloUnit(
            std::initializer_list<ReservationStationGroup*> res_groups) {
        
        for(auto rsg : res_groups)
            this->res_stations.push_back(rsg);
    }
};

#endif // __JJC__TOMASULO__UNIT__H__