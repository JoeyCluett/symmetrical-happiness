#pragma once

#include <queue>
#include <stdexcept>
#include "InstructionQueue.h"
typedef InstructionQueueEntry instruction_t;

class ReorderBuffer {
private:
    struct buffer_entry_t {
        instruction_t inst;
        int index;  
    };

    buffer_entry_t* buffer;
    int used_entries = 0;
    int max_buffer_size = 0;

    // needed for proper ROB operation
    int issue_pointer = 0;  // next available slot
    int commit_pointer = 0; // 

public:
    ReorderBuffer(int buffer_size) {
        this->max_buffer_size = buffer_size;
        this->buffer = new buffer_entry_t[buffer_size];
    }

    bool hasFreeEntry(void) {
        return used_entries < max_buffer_size;
    }

    

    int placeInstruction(instruction_t ins, int index) {
        if(used_entries >= max_buffer_size)
            throw std::runtime_error("Reorder Buffer has no free openings");

        buffer[issue_pointer] = {ins, index};

        used_entries++;
        issue_pointer++;
        issue_pointer %= this->max_buffer_size;
    }

};

