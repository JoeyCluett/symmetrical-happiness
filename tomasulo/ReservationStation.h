#ifndef __JJC__RESERVATION__STATION__H__
#define __JJC__RESERVATION__STATION__H__

#include <vector>

// maintains a global record of all reservation stations
class ReservationStationEntry {
public:
    // memory leak waiting to happen
    static std::vector<ReservationStationEntry*> station_entries;
    int index;

public:
    ReservationStationEntry(void) {
        this->index = station_entries.size();
        station_entries.push_back(this);
    }

    int getIndex(void) {
        return this->index;
    }

    int busy = false;
    int operation;
    int Vj;
    int Vk;
    int Qj;
    int Qk;
    int cycles;
    int disp;
};
typedef ReservationStationEntry rstation_entry_t;
std::vector<rstation_entry_t*> ReservationStationEntry::station_entries = std::vector<rstation_entry_t*>();

class ReservationStationGroup {
private:
    std::vector<int> rs_indices;
    
    // instructions take a specific amount of time
    std::vector<int> ops;
    std::vector<int> timing;

public:
    int entries;

    ReservationStationGroup(int entries,
            std::initializer_list<int> ops,
            std::initializer_list<int> timing) {

        for(int i = 0; i < entries; i++) {
            auto entry = new rstation_entry_t();
            this->rs_indices.push_back(entry->getIndex());
        }

        this->ops = ops;
        this->timing = timing;
    }

    // does this reservation station group have a free entry
    // return -1 if no
    int freeStation(void) {
        for(auto i : rs_indices) {
            if(ReservationStationEntry::station_entries.at(i)->busy 
                    == false) {
                return i;
            }
        }
        return -1;
    }
};
typedef ReservationStationGroup rstation_group_t;

#endif // __JJC__RESERVATION__STATION__H__