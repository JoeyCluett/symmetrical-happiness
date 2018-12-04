#pragma once

#include <utility>
#include <stdexcept>
typedef std::pair<int, int> buffer_entry_t; // <pc of instruction, ret_value>

class reorder_buffer {
private:
    buffer_entry_t entry_arr[10];

    int issue_pointer  = 0;
    int commit_pointer = 0;
    int _size = 0;

public:

    buffer_entry_t& operator[](int index) {
        return this->entry_arr[index];
    }

    void increment_commit_pointer(void) {
        this->commit_pointer++;
        this->commit_pointer %= 10;
    }

    void increment_issue_pointer(void) {
        this->issue_pointer++;
        this->issue_pointer %= 10;
    }

    void issue_instruction(int program_counter) {
        int next_entry = this->next_available_entry();
        if(next_entry != -1) {
            entry_arr[next_entry].first = program_counter;
            this->increment_issue_pointer();
            this->_size++;
        }
    }

    // returns -1 if no entry available
    int next_available_entry(void) {
        if(_size == 10)
            return -1;
        return commit_pointer;
    }

};