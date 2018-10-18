#ifndef __JJS__REGISTER__FILE__H__
#define __JJS__REGISTER__FILE__H__

#include <iostream>
#include <string>
#define NOT_USED -1 // RAT entries for un-allocated registers

union MultiType {
    int i;
    float f;
};

class RegisterEntry {
public:
    MultiType rf; // value
    int rat 
        //= 1;
        = NOT_USED;
};
typedef RegisterEntry reg_entry_t;

class RegisterFile {
private:
    reg_entry_t* regs;
    int num_entries;

    std::string int_with_length(int val, int width) {
        std::string v = std::to_string(val);
        while(v.size() < width) {
            v.push_back(' ');
        }
        return v;
    }

public:
    RegisterFile(int entries) {
        regs = new reg_entry_t[entries];
        this->num_entries = entries;
    }

    // checks entry against number of registers
    bool set_reg(int entry, int rf, int rat) {
        if(entry >= num_entries)
            return false;
        regs[entry].rf.i  = rf;
        regs[entry].rat = rat;
        return true;
    }

    // easy to print the entire regsiter file
    friend std::ostream& operator<<(std::ostream& os, RegisterFile& rf) {
        os << "   RF              RAT\n";
        os << "-------------------------\n";
        for(int i = 0; i < rf.num_entries; i++) {
            os << i << ": " << rf.int_with_length(rf.regs[i].rf.i, 11);
            if(rf.regs[i].rat != NOT_USED)
                os << "     RS" << rf.regs[i].rat;
            else
                os << "     NOT_USED";
            os << std::endl;
        }
        return os;
    }

    ~RegisterFile() {
        delete[] regs; // polite to clean up after ourselves
    }
};
typedef RegisterFile reg_file_t;

#endif // __JJS__REGISTER__FILE__H__