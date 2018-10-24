#ifndef __JJS__REGISTER__FILE__H__
#define __JJS__REGISTER__FILE__H__

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