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

//#define __MIPSRT_DEBUG__ // see instruction names anf registers as they execute

#define MAX_LINEAR_MEM_SIZE (1024*1024) // a full MB

union MIPS_REGISTER {
    int i32;
    float f32;
    uint32_t u32;
};

class MipsRuntime {
private:
    std::map<int, MIPS_REGISTER> register_file;
    std::map<addr_t, int> memory;
    addr_t PC = 0;

    std::vector<BranchOccurance> branch_history;
    bool track_branches = false;

    MipsTokenizer* mt_ptr = NULL;

public:
    MipsRuntime(MipsTokenizer& mt) {
        this->memory.clear();
        this->register_file.clear();
        this->mt_ptr = &mt;
    }

    void trackBranches(bool track_branches) {
        this->track_branches = track_branches;
    }

    auto getBranchHistory(void) -> std::vector<BranchOccurance>& {
        return this->branch_history;
    }

    void peekRegister(std::string reg_name) {
        int reg_index = mt_ptr->registerOffset(reg_name);
        std::cout << reg_name << " : " << register_file.at(reg_index).i32 << std::endl;
    }

    // show all registers
    void peekRegister(void) {
        for(auto& l : register_file) {
            std::string reg_name = mt_ptr->registerName(l.first);
            std::cout << '{' << reg_name << " : " << l.second.i32 << "} ";
        }
        std::cout << std::endl;
    }

    // show every entry in memory
    void peekMemory(void) {
        for(auto& l : this->memory) {
            std::cout << '{' << l.first << " : " << l.second << "} ";
        }
        std::cout << std::endl;
    }

    void pokeRegister(std::string reg_name, int val) {
        register_file.at(mt_ptr->registerOffset(reg_name)).i32 = val;
    }

    void pokeRegister(std::string reg_name, float val) {
        this->pokeRegister(reg_name, *(int*)&val);
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

    void peekMemory_i32(addr_t address, int n = 1) {
        for(int i = 0; i < n; i++)
            std::cout << this->memory[address + (i << 2)] << ' ';
    }

    // returns whether program has exited
    bool execute(MipsTokenizer& mt, int cycles, addr_t starting_address, bool wait_cycles = false) {
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
                    register_file[mi.argv[0]].i32 = memory[register_file[mi.argv[2]].i32 + mi.argv[1]];
                    PC++;

                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "lw " << mt.registerName(mi.argv[0]) 
                    << ' ' << mi.argv[1] << '(' << mt.registerName(mi.argv[2]) 
                    << ')' << std::endl;
                    #endif

                    break;

                case MIPS_inst::sw:
                    memory[register_file[mi.argv[2]].i32 + mi.argv[1]] = register_file[mi.argv[0]].i32;
                    PC++;

                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "sw " << mt.registerName(mi.argv[0]) 
                    << ' ' << mi.argv[1] << '(' << mt.registerName(mi.argv[2]) 
                    << ')' << std::endl;
                    #endif

                    break;

                case MIPS_inst::addi:
                    register_file[mi.argv[0]].i32 = register_file[mi.argv[1]].i32 + mi.argv[2];
                    PC++;

                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "addi " << mt.registerName(mi.argv[0]) 
                    << ' ' << mt.registerName(mi.argv[1]) << ' ' << mi.argv[2]
                    << '\n';
                    #endif

                    break;

                case MIPS_inst::bne:
                    {
                        bool branch = (register_file[mi.argv[0]].i32 
                                != register_file[mi.argv[1]].i32);
                        
                        if(track_branches)  
                            this->branch_history.push_back(BranchOccasion(starting_address 
                            + (PC << 2), branch));
                        
                        #ifdef __MIPSRT_DEBUG__
                        std::cout << "bne " << mt.registerName(mi.argv[0]) 
                        << ' ' << mt.registerName(mi.argv[1]) 
                        << ' ' << mt.jumpName(mi.argv[2]) 
                        << (branch ? " -> TAKEN" : " -> NOT TAKEN") << std::endl;
                        #endif // __MIPSRT_DEBUG__

                        if(branch) {
                            PC = *(uint32_t*)&mi.argv[2];
                        } else {
                            PC++;
                        }
                    }

                    break;

                case MIPS_inst::slt:
                    register_file[mi.argv[0]].i32 = 0;
                    if(register_file[mi.argv[1]].i32 < register_file[mi.argv[2]].i32)
                        register_file[mi.argv[0]].i32 = 1;
                    PC++;

                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "slt ";
                    LOOP(i, 0, 3)
                        std::cout << mt.registerName(mi.argv[i]) << ' ';
                    std::cout << std::endl;
                    #endif

                    break;

                case MIPS_inst::b:
                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "b\n";
                    #endif

                    throw std::runtime_error("MIPS_inst::b : THIS INSTRUCTION IS NOT DEFINED");
                
                case MIPS_inst::beq:
                    {
                        bool branch = (register_file[mi.argv[0]].i32 
                                == register_file[mi.argv[1]].i32);

                        if(track_branches)
                            this->branch_history.push_back(BranchOccasion(starting_address + 
                            (PC << 2), branch));

                        #ifdef __MIPSRT_DEBUG__
                        std::cout << "beq " << mt.registerName(mi.argv[0]) 
                        << ' ' << mt.registerName(mi.argv[1]) 
                        << ' ' << mt.jumpName(mi.argv[2]) 
                        << (branch ? " -> TAKEN" : " -> NOT TAKEN") << std::endl;
                        #endif // __MIPSRT_DEBUG__

                        if(branch) {
                            PC = *(uint32_t*)&mi.argv[2];
                        } else {
                            PC++;
                        }
                    }

                    break;

                case MIPS_inst::halt:
                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "halt\n";
                    #endif // __MIPSRT_DEBUG__

                    return true; // CPU has halted execution
                    break;

                default:
                    throw std::runtime_error("UNKNOWN OPCODE: " + std::to_string(mi.opcode));
            }

            if(wait_cycles) {
                // wait for user input to advance
                getch();
            }

        }

        return false;
    }

};

#endif // __JJC__MIPS__RUNTIME__H__
