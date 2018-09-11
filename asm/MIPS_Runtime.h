#ifndef __JJC__MIPS__RUNTIME__H__
#define __JJC__MIPS__RUNTIME__H__

#include <map>
#include <vector>
#include <stdexcept>

#include "MIPS_Tokenizer.h"
#include "MIPS_Constants.h"
#include <getch.h>

#define __MIPSRT_DEBUG__ // see instruction names as they execute

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
            return tmp;

        } else {
            // std::map access
            int tmp = this->non_linear_mem[addr];
            this->non_linear_mem[addr] = data;
            return tmp;
        }
    }

    void poke(addr_t start_addr, std::vector<int> data) {
        for(int i = 0; i < data.size(); i++)
            this->poke(start_addr+i, data[i]);
    }

    int setIntReg(int reg, int value) {
        int tmp = this->int_reg_file[reg];
        this->int_reg_file[reg] = value;
        return tmp;
    }

    int fetchMem(addr_t addr) {
        addr -= this->mem_starting_address;
        if(this->linear) {
            return this->linear_mem.at(addr);
        } else {
            return this->non_linear_mem[addr];
        }
    }

    void execute(MipsTokenizer& mt, int cycles, bool wait_cycles = false) {
        for(int PC = 0; PC < cycles;) {
            MipsInstruction mi = mt[PC];

            switch(mi.opcode) {
                case MIPS_inst::add:
                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "add\n";
                    #endif

                    int_reg_file[mi.argv[0]] = int_reg_file[mi.argv[1]] + int_reg_file[mi.argv[2]];
                    PC++;
                    break;

                case MIPS_inst::lw:
                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "lw\n";
                    #endif

                    this->setIntReg(mi.argv[0], this->fetchMem(int_reg_file[mi.argv[2]] + mi.argv[1]));
                    PC++;
                    break;

                case MIPS_inst::sw:
                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "sw\n";
                    #endif

                    this->poke(int_reg_file[mi.argv[2]] + mi.argv[1], int_reg_file[mi.argv[0]]);
                    PC++;
                    break;

                case MIPS_inst::addi:
                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "addi\n";
                    #endif

                    int_reg_file[mi.argv[0]] = int_reg_file[mi.argv[1]] + mi.argv[2];
                    PC++;
                    break;

                case MIPS_inst::bne:
                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "bne\n";
                    #endif

                    if(mi.argv[0] != mi.argv[1]) {
                        PC = mi.argv[2];
                    } else {
                        PC++;
                    }
                    break;

                case MIPS_inst::slt:
                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "slt\n";
                    #endif

                    if(int_reg_file[mi.argv[1]] < int_reg_file[mi.argv[2]]) {
                        int_reg_file[mi.argv[0]] = 1;
                    } else {
                        int_reg_file[mi.argv[0]] = 0;
                    }
                    PC++;
                    break;

                case MIPS_inst::b:
                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "b\n";
                    #endif

                    throw std::runtime_error("MIPS_inst::b : THIS INSTRUCTION IS NOT DEFINED");
                
                case MIPS_inst::beq:
                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "beq\n";
                    #endif

                    if(mi.argv[0] == mi.argv[1]) {
                        PC = mi.argv[2];
                    } else {
                        PC++;
                    }
                    break;

                default:
                    throw std::runtime_error("UNKNOWN OPCODE: " + std::to_string(mi.opcode));
            }

            if(wait_cycles) {
                // wait for user input to advance
                getch();
            }

        }
    }

};

#endif // __JJC__MIPS__RUNTIME__H__
