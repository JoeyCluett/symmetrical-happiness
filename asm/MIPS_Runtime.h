#ifndef __JJC__MIPS__RUNTIME__H__
#define __JJC__MIPS__RUNTIME__H__

#include <map>
#include <vector>
#include <stdexcept>

#include "MIPS_Tokenizer.h"
#include "MIPS_Constants.h"
#include <BranchPredictor.h>
#include <getch.h>
#include <jjc_macros.h>

#define __MIPSRT_DEBUG__ // see instruction names as they execute

#define MAX_LINEAR_MEM_SIZE (1024*1024) // a full MB

union MIPS_REGISTER {
    int i32;
    float f32;
};

class MipsRuntime {
private:
    std::map<int, MIPS_REGISTER> register_file;
    std::map<addr_t, int> memory;
    addr_t PC = 0;

    std::vector<BranchOccurance> branch_history;
    bool track_branches = false;

public:
    MipsRuntime(void) {
        this->memory.clear();
        this->register_file.clear();
    }

    void pokeReg_i32(std::string reg_name, int val) {
        
    }

    void pokeMemory_i32(addr_t address, int data) {
        this->memory[address] = data;
    }

    void pokeMemory_i32(addr_t start_address, std::vector<int> data) {
        addr_t offset = 0L;
        for(auto n : data) {
            this->memory[start_address + offset] = n;
            offset += 4;
        }
    }

    void pokeMemory_f32(addr_t address, float data) {
        this->memory[address] = *(int*)&data;
    }

    void pokeMemory_f32(addr_t start_address, std::vector<float> data) {
        addr_t offset = 0L;
        for(auto n : data) {
            this->memory[start_address + offset] = n;
            offset += 4;
        }
    }

    void peekMemory_i32(addr_t address, int n = 1) {
        for(int i = 0; i < n; i++)
            std::cout << this->memory[address + (i << 2)] << ' ';
    }

    void execute(MipsTokenizer& mt, int cycles, addr_t starting_address, bool wait_cycles = false) {
        for(int i = 0; i < cycles; i++) {
            MipsInstruction mi = mt[PC];

            switch(mi.opcode) {
                case MIPS_inst::add:
                    register_file[mi.argv[0]].i32 = register_file[mi.argv[1]].i32 + register_file[mi.argv[2]].i32;
                    PC++;

                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "add ";
                    LOOP(i, 0, 3)
                        std::cout << mt.registerName(mi.argv[i]) << ' ';
                    std::cout << std::endl;
                    #endif

                    break;

                case MIPS_inst::lw:
                    //this->setIntReg(mi.argv[0], this->fetchMem(int_reg_file[mi.argv[2]] + mi.argv[1]));
                    register_file[mi.argv[0]].i32 = memory[register_file[mi.argv[2]].i32 + mi.argv[1]];
                    PC++;

                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "lw " << mt.registerName(mi.argv[0]) 
                    << ' ' << mi.argv[1] << '(' << mt.registerName(mi.argv[2]) 
                    << ')' << std::endl;
                    #endif

                    break;

                case MIPS_inst::sw:
                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "sw\n";
                    #endif

                    //this->pokeMem(int_reg_file[mi.argv[2]] + mi.argv[1], int_reg_file[mi.argv[0]]);
                    pokeMemory_i32(register_file[mi.argv[2]].i32 + mi.argv[1], register_file[mi.argv[0]].i32);

                    PC++;
                    break;

                case MIPS_inst::addi:
                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "addi\n";
                    #endif

                    register_file[mi.argv[0]].i32 = register_file[mi.argv[1]].i32 + mi.argv[2];
                    PC++;
                    break;

                case MIPS_inst::bne:
                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "bne\n";
                    #endif

                    {
                        bool branch = (mi.argv[0] != mi.argv[1]);
                        if(track_branches)  
                            this->branch_history.push_back(BranchOccasion(starting_address +(PC << 2), branch));
                        
                        if(branch) {
                            PC = mi.argv[2];
                        } else {
                            PC++;
                        }
                    }

                    break;

                case MIPS_inst::slt:
                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "slt\n";
                    #endif

                    register_file[mi.argv[0]].i32 = 0;
                    if(register_file[mi.argv[1]].i32 < register_file[mi.argv[2]].i32)
                        register_file[mi.argv[0]].i32 = 1;

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
                    #endif // __MIPSRT_DEBUG__

                    {
                        bool branch = (mi.argv[0] == mi.argv[1]);
                        if(track_branches)
                            this->branch_history.push_back(BranchOccasion(starting_address + (PC << 2), branch));

                        if(branch) {
                            PC = mi.argv[2];
                        } else {
                            PC++;
                        }
                    }

                    break;

                case MIPS_inst::halt:
                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "halt\n";
                    #endif // __MIPSRT_DEBUG__

                    std::cout << "MIPS_inst::halt\n";
                    return; // CPU has halted execution
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
