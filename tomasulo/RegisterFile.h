#ifndef __JJS__REGISTER__FILE__H__
#define __JJS__REGISTER__FILE__H__

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

#include <iostream>
#include <string>
#include <stdexcept>
#define NOT_USED -1 // RAT entries for un-allocated registers

union MultiType {
    int i;
    float f;

    MultiType(int i) {
        this->i = i;
    }

    MultiType(float f) {
        this->f = f;
    }

    MultiType(void) { }
};

class RegisterEntry {
public:
    MultiType rf; // value
    int rat 
        //= 1;
        = NOT_USED;

    friend std::ostream& operator<<(std::ostream& os, RegisterEntry& re) {
        os << "rf: [" << re.rf.i << " / " << re.rf.f << "], ";
        os << "rat: ";
        if(re.rat == NOT_USED)
            os << "NOT_USED";
        else 
            os << "RS" << re.rat;
        os << std::endl;
        return os; 
    }

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

    void verifyRegisterIndex(int index) {
        if(index >= num_entries || index < 0)
            throw std::runtime_error("RegisterFile::verifyRegisterIndex() -> invalid register index: "
            + std::to_string(index));
    }

public:
    RegisterFile(int entries) {
        regs = new reg_entry_t[entries];
        this->num_entries = entries;
    }

    int rfSize(void) {
        return this->num_entries;
    }

    bool allocated(int register_entry) {
        verifyRegisterIndex(register_entry);

        if(regs[register_entry].rat == NOT_USED)
            return false;
        return true;
    }

    RegisterEntry* getRegister(int index) {
        verifyRegisterIndex(index);
        //std::cout << "RegisterFile::getRegister() -> register index: " << index << std::endl;
        //std::cout << "    " << regs[index] << std::endl;
        return regs + index;
    }

    // checks entry against number of registers
    bool set_reg(int entry, int rf, int rat) {
        verifyRegisterIndex(entry);

        regs[entry].rf.i  = rf;
        regs[entry].rat = rat;
        return true;
    }

    // easy to print the entire register file
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