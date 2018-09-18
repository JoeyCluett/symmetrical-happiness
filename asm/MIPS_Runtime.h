#ifndef __JJC__MIPS__RUNTIME__H__
#define __JJC__MIPS__RUNTIME__H__

#include <map>
#include <vector>
#include <stdexcept>

#include "MIPS_Tokenizer.h"
#include "MIPS_Constants.h"
#include <BranchPredictor.h>
#include <getch.h>

//#define __MIPSRT_DEBUG__ // see instruction names as they execute

#define MAX_LINEAR_MEM_SIZE (1024*1024) // a full MB

class MipsRuntime {
private:
    int   int_reg_file[32];
    float float_reg_file[32];

    bool linear = false; // this is what you will want MOST of the time

    addr_t mem_start_address = 0L;
    addr_t inst_start_address = 0L;

    std::map<addr_t, int> non_linear_mem; // recommended if program elements are far apart
    std::vector<int>      linear_mem;

    addr_t PC = 0;

    std::vector<BranchOccasion> branch_history;
    bool track_branches = false;

public:
    MipsRuntime(void) {
        // zero out all registers
        for(int i = 0; i < 32; i++) {
            this->int_reg_file[i]   = 0;
            this->float_reg_file[i] = 0;
        }
    }

    int& reg(int index) {
        return int_reg_file[index];
    }

    float& fpReg(int index) {
        return float_reg_file[index];
    }

    void setStartInstructionAddress(addr_t address) {
        this->inst_start_address = address;
    }

    void setStartDataAddress(addr_t address) {
        this->mem_start_address = address;
    }

    void setProgramLinearity(bool linear) {
        this->linear = linear;
    }

    void setTrackBranches(bool track_branches) {
        this->track_branches = track_branches;
    }

    auto getBranchHistory(void) -> std::vector<BranchOccasion>& {
        return this->branch_history;
    }

    auto peekMem(addr_t addr, int n) -> std::string {
        std::string ret = "{";
        for(int i = 0; i < (n-1); i++) {
            ret += std::to_string(this->fetchMem(addr + i));
            ret += std::string(", ");
        }
        ret += std::to_string(this->fetchMem(addr + n - 1));
        ret += std::string("}");
        return ret;
    }

    int pokeMem(addr_t addr, int data) {
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

    void pokeMem(addr_t start_addr, std::vector<int> data) {
        for(int i = 0; i < data.size(); i++)
            this->pokeMem(start_addr+i, data[i]);
    }

    int setIntReg(int reg, int value) {
        int tmp = this->int_reg_file[reg];
        this->int_reg_file[reg] = value;
        return tmp;
    }

    int fetchMem(addr_t addr) {
        #ifdef __MIPSRT_DEBUG__
        std::cout << "fetchMem ";
        #endif // __MIPSRT_DEBUG__

        if(this->linear) {
            return this->linear_mem.at(addr);
        } else {
            return this->non_linear_mem[addr];
        }
    }

    addr_t getEffectiveAddress(addr_t trunc_address) {
        return ((trunc_address << 2) + this->inst_start_address);
    }

    void execute(MipsTokenizer& mt, int cycles, bool wait_cycles = false) {
        for(int i = 0; i < cycles; i++) {
            MipsInstruction mi = mt[PC];

            #ifdef __MIPSRT_DEBUG__
            std::cout << std::hex << getEffectiveAddress(PC) << std::dec << " (" << PC << ") : ";
            #endif // __MIPSRT_DEBUG__

            switch(mi.opcode) {
                case MIPS_inst::add:
                    int_reg_file[mi.argv[0]] = int_reg_file[mi.argv[1]] + int_reg_file[mi.argv[2]];
                    PC++;

                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "add\n";
                    #endif

                    break;

                case MIPS_inst::lw:
                    this->setIntReg(mi.argv[0], this->fetchMem(int_reg_file[mi.argv[2]] + mi.argv[1]));
                    PC++;

                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "lw\n";
                    #endif

                    break;

                case MIPS_inst::sw:
                    #ifdef __MIPSRT_DEBUG__
                    std::cout << "sw\n";
                    #endif

                    this->pokeMem(int_reg_file[mi.argv[2]] + mi.argv[1], int_reg_file[mi.argv[0]]);
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

                    {
                        bool branch = (mi.argv[0] != mi.argv[1]);
                        if(track_branches)  
                            this->branch_history.push_back(BranchOccasion(getEffectiveAddress(PC), branch));
                        
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
                    #endif // __MIPSRT_DEBUG__

                    {
                        bool branch = (mi.argv[0] == mi.argv[1]);
                        if(track_branches)
                            this->branch_history.push_back(BranchOccasion(getEffectiveAddress(PC), branch));

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
