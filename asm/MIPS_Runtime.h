#ifndef __JJC__MIPS__RUNTIME__H__
#define __JJC__MIPS__RUNTIME__H__

#include <map>
#include <vector>
#include "MIPS_Tokenizer.h"

#define MAX_LINEAR_MEM_SIZE (1024*1024) // a full MB

typedef int64_t addr_t;

class MipsRuntime {
private:
    int   int_reg_file[32];
    float float_reg_file[32];

    bool linear;
    addr_t mem_starting_address;

    std::map<addr_t, int> non_linear_mem; // recommended if sparse program
    std::vector<int>      linear_mem;

public:
    MipsRuntime(addr_t inst_starting_address = 0, bool linear = false) {
        // zero out all registers
        for(int i = 0; i < 32; i++) {
            this->int_reg_file[i]   = 0;
            this->float_reg_file[i] = 0;
        }

        this->mem_starting_address = inst_starting_address;
        this->linear = linear;
    }

    int poke(addr_t addr, int data) {
        addr -= mem_starting_address;
        if(this->linear) {
            // std::vector access
            while(this->linear_mem.size() <= addr && this->linear_mem.size() < MAX_LINEAR_MEM_SIZE)
                this->linear_mem.push_back(0);

            int tmp = this->linear_mem[addr];
            linear_mem[addr] = data;

        } else {
            // std::map access
            int tmp = this->non_linear_mem[addr];
            this->non_linear_mem[addr] = data;
            return tmp;
        }
    }

    int poke(addr_t start_addr, std::vector<int> data) {
        for(int i = 0; i < data.size(); i++)
            this->poke(start_addr+i, data[i]);
    }

};

#endif // __JJC__MIPS__RUNTIME__H__