#ifndef __JJC__INSTRUCTION__QUEUE__H__
#define __JJC__INSTRUCTION__QUEUE__H__

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
                throw std::runtime_error("UNKNOWN OPCODE IN InstructionQueueEntry: " 
                    + std::to_string(iqe.opcode));
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
    int instruction_pointer; // used internally

public:
    // easy printing later on
    friend std::ostream& operator<<(std::ostream& os, InstructionQueue& iq) {
        os << "Number of instructions: " << iq.iq_entry_vec.size() << std::endl;

        for(int i = 0; i < iq.iq_entry_vec.size(); i++) {
            os << iq.iq_entry_vec.at(i);
            if(i == iq.instruction_pointer) {
                os << " <-- NEXT INSTRUCTION\n";
            } else {
                os << "\n";
            }
        
        }

        //for(auto& in : iq.iq_entry_vec)
            //os << in << std::endl;

        return os;
    }

    bool hasNextInstruction(void) {
        if(instruction_pointer >= iq_entry_vec.size())
            return false;
        return true;
    }

    iq_entry_t& getNextInstruction(void) {
        return iq_entry_vec.at(instruction_pointer);
    }

    void advanceIPtr(void) {
        this->instruction_pointer++;
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

        this->instruction_pointer = 0;
    }
};

#endif // __JJC__INSTRUCTION__QUEUE__H__