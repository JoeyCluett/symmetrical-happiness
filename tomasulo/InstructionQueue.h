#ifndef __JJC__INSTRUCTION__QUEUE__H__
#define __JJC__INSTRUCTION__QUEUE__H__

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

#include "RegisterFile.h"
#include "Constants.h"

class InstructionQueueEntry {
public:
    int opcode;
    int dest;
    int source[2];

    const char* opcode_name_lut[4] = {
        "add",
        "sub",
        "mul",
        "div"
    };

    friend std::ostream& operator<<(std::ostream& os, InstructionQueueEntry& iqe) {
        //os << iqe.opcode_name_lut[iqe.opcode] << ' ' << iqe.dest << " <- " << 
        //iqe.source[0] << ' ' << iqe.source[1];
        os << "R[" << iqe.dest << "] = R[" << iqe.source[0];

        switch(iqe.opcode) {
            case ADD:
                os << "] + "; break;
            case SUB:
                os << "] - "; break;
            case MUL:
                os << "] * "; break;
            case DIV:
                os << "] / "; break;
            default:
                throw std::runtime_error("UNKNOWN OPCODE IN InstructionQueueEntry: " + std::to_string(iqe.opcode));
                break;
        }
        
        os << "R[" << iqe.source[1] << "]";

        return os;
    }

};
typedef InstructionQueueEntry iq_entry_t;

class InstructionQueue {
private:
    std::vector<iq_entry_t> iq_entry_vec;

public:
    // easy printing later on
    friend std::ostream& operator<<(std::ostream& os, InstructionQueue& iq) {
        os << "Number of instructions: " << iq.iq_entry_vec.size() << std::endl;

        for(auto& in : iq.iq_entry_vec)
            os << in << std::endl;

        return os;
    }

    InstructionQueue(std::string filename, reg_file_t& reg_file) {
        // default mode is formatted input
        std::ifstream is(filename);

        const int STATE_number_instructions = 0;
        const int STATE_sim_cycles          = 1;
        const int STATE_instruction_inputs  = 2;
        const int STATE_register_start      = 3;
        const int STATE_end = -1;

        int current_state = STATE_number_instructions;
        
        int number_of_instructions = -1;
        int simulation_cycles = -1;

        while(current_state != STATE_end) {
            switch(current_state) {
                case STATE_number_instructions:
                    is >> number_of_instructions;
                    current_state = STATE_sim_cycles;
                    break;

                case STATE_sim_cycles:
                    is >> simulation_cycles;
                    current_state = STATE_instruction_inputs;
                    break;

                case STATE_instruction_inputs:
                    {
                        for(int i = 0; i < number_of_instructions; i++) {
                            iq_entry_t entry;
                            is >> entry.opcode;
                            is >> entry.dest;
                            is >> entry.source[0];
                            is >> entry.source[1];

                            this->iq_entry_vec.push_back(entry);
                        }
                        current_state = STATE_register_start;
                    }
                    break;

                case STATE_register_start:
                    for(int i = 0; i < 8; i++) {
                        int v;
                        is >> v;
                        if(!reg_file.set_reg(i, v, NOT_USED))
                            throw std::runtime_error("Invalid register index");
                    }
                    current_state = STATE_end;
                    break;

                default:
                    throw std::runtime_error("Unknow state in InstructionQueue::constructor()");
            }
        }
    }
};

#endif // __JJC__INSTRUCTION__QUEUE__H__