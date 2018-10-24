#ifndef __JJC__RESERVATION__STATION__H__
#define __JJC__RESERVATION__STATION__H__

#include <vector>
#include <stdexcept>
#include "InstructionQueue.h"

// maintains a global record of all reservation stations
class ReservationStationEntry {
public:
    // memory leak waiting to happen (I'm sorry)
    // this is a global table of all ReservationStationEntry objects
    static std::vector<ReservationStationEntry*> station_entries;

    // each entry maintains its own index into the station entries table
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
    int hadIssue = false;
    int operation;
    int Vj;
    int Vk;
    int Qj;
    int Qk;
    int cycles;
    int disp;

    void reset(void) {
        operation = -1;
        Vj = 0;
        Vk = 0;
        Qj = -1;
        Qk = -1;
        busy = false;
        hadIssue = false;

        cycles = 0;
    }

    friend std::ostream& operator<<(std::ostream& os, ReservationStationEntry& rse) {
    
        if(rse.busy == false) {
            os << operation_lut.back();
        } else {
            os << operation_lut.at(rse.operation);
        }

        auto bSize = [](int i, int sz) -> std::string {
            auto str = std::to_string(i);
            while(str.size() < sz) {
                str += " ";
            }
            return str;
        };

        for(int i : {rse.Vj, rse.Vk}) {
            os << "| " << bSize(i, 10);
        }

        // print each integer in the correct size
        for(int i : {rse.Qj, rse.Qk}) {
            os << "| ";
            if(i == -1) {
                os << "XXX       ";
            } else {
                os << "RS" << bSize(i, 8);
            }
        }

        return os;
    }
};
typedef ReservationStationEntry rstation_entry_t;
std::vector<rstation_entry_t*> ReservationStationEntry::station_entries = std::vector<rstation_entry_t*>();

#include "ExecutionUnit.h"

class ReservationStationGroup {
private:
    std::vector<int> rs_indices;
    
    // instructions take a specific amount of time
    std::vector<int> ops;
    std::vector<int> timing;

    ExecutionUnit eu; // European Union 

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
        this->entries = ops.size();
    }

    int timingForOperation(int operation) {
        for(int i = 0; i < ops.size(); i++) {
            if(ops[i] == operation)
                return timing.at(i);
        }

        throw std::runtime_error("ReservationStationGroup::timingForOperation() -> unknown operation");
    }

    // tests for any operation defined in the Constants.h file
    bool supportsOperation(int operation) {
        for(int i : this->ops) {
            if(operation == i)
                return true;
        }
        return false;
    }

    // assume Tomasulo has already tested for a free station
    // throw std::runtime_error otherwise
    void issue(iq_entry_t& iq_entry, RegisterFile* rf) {
        // returns index into global reservation station table
        int free_station = this->freeStation();

        if(free_station == -1)
            throw std::runtime_error("ReservationStationGroup::dispatch() -> station not available");
        else {
            // there is only one storage space for all reservation stations
            auto ptr = rstation_entry_t::station_entries.at(free_station);

            // now ptr is the actual reservation station
            ptr->operation = iq_entry.opcode;

            auto reg = rf->getRegister(iq_entry.source[0]);
            if(reg->rat == NOT_USED) {
                ptr->Vj = reg->rf.i;
            } else {
                ptr->Qj = reg->rat;
            }

            reg = rf->getRegister(iq_entry.source[1]);
            if(reg->rat == NOT_USED) {
                ptr->Vk = reg->rf.i;
            } else {
                ptr->Qk = reg->rat;
            }

            // destination register
            ptr->disp = iq_entry.dest;

            // register file needs to be updated
            //std::cout << "Instruction destination: R" << iq_entry.dest << std::endl;
            reg = rf->getRegister(iq_entry.dest);
            reg->rat = free_station;
            ptr->busy = true;
            ptr->cycles = 0;
            ptr->hadIssue = true;
        }
    }

    // looks for an entry that is ready to execute and 
    // pushes that entry to the execution unit. releases 
    // the reservation station. returns whether issue was possible
    bool dispatch(void) {
        if(this->eu.busy == true)
            return false;

        // execution unit can accept an instruction to execute
        for(int i : rs_indices) {
            auto& rs = rstation_entry_t::station_entries[i];
            if(rs->Qj == -1 && rs->Qk == -1 && rs->operation != -1 && rs->hadIssue == false) {
                // instruction can be issued. someday I will implement 
                // a cycle counter that will issue the oldest 
                // instruction that can be executed

                //std::cout << "ReservationStationGroup::dispatch()-> " 
                //<< rs->operation << std::endl;
                this->eu.cycles_to_complete = timingForOperation(rs->operation);
                this->eu.input_1 = rs->Vj; // i am almost definitely screwing something up
                this->eu.input_2 = rs->Vk;
                this->eu.busy = true;
                this->eu.operation = rs->operation;
                this->eu.dest_reg = rs->disp; // destination register
                this->eu.readyToBroadcast = false;

                rs->reset();
                return true;
            }
        }

        // could not issue an instruction to execution unit
        return false;
    }

    // does this reservation station group have a free entry?
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

    // reset every reservation station entry referenced 
    // by this object
    void reset(void) {
        for(int i : this->rs_indices) {
            rstation_entry_t::station_entries.at(i)->reset();
        }
        this->eu.operation = NONE;
    }

    void advanceExecutionUnit(void) {
        if(eu.current_cycles == eu.cycles_to_complete) {
            eu.readyToBroadcast = true;
            return;
        }
        else if(this->eu.busy && this->eu.readyToBroadcast == false)
            this->eu.current_cycles++;
    }

    friend std::ostream& operator<<(std::ostream& os, ReservationStationGroup& rsg) {
        os << "   Operations: ";
        for(int i : rsg.ops) {
            os << op_symbol_lut.at(i) << ' ';
        }

        os << "\n-----------------------------------------------------------\n";
        os <<   "# | Operation  | Vj        | Vk        | Qj        | Qk";
        os << "\n-----------------------------------------------------------\n";

        for(int i : rsg.rs_indices) {
            os << i << " | " << *rstation_entry_t::station_entries.at(i) << std::endl;
        }
        os << std::endl;

        os << "+----\n";
        os << "| " << rsg.eu;
        os << "+----\n\n";

        return os;
    }
};
typedef ReservationStationGroup rstation_group_t;

#endif // __JJC__RESERVATION__STATION__H__