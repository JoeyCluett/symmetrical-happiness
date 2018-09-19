#ifndef __JJC__MIPS__TOKENIZER__H__
#define __JJC__MIPS__TOKENIZER__H__

#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <stdexcept> // std::runtime_exception()
#include "MIPS_Constants.h"

//#define __DEBUG__

class MipsInstruction {
public:
    int opcode;
    int argv[3];
}; // 16 bytes per intermediate instruction may be excessive

class MipsTokenizer {
private:
    std::vector<MipsInstruction> instruction_stream;
    std::vector<int> jump_table;
    std::map<std::string, int> jump_offset_table;

    std::map<std::string, int> reg_name_to_int;
    std::vector<std::string>   int_to_reg_name;

    auto cleanSource(std::string filename) -> std::string; // returns new filename
    auto finalizeJumpTargets(void) -> void; // turn jump offsets into targets

public:
    MipsTokenizer(std::string filename);

    int size(void) {
        return this->instruction_stream.size();
    }

    int strToOpcode(std::string op_str);
    
    int registerOffset(std::string reg_name);

    auto operator[](int index) -> MipsInstruction& {
        return this->instruction_stream.at(index); // safe access
    }

    auto at(int index) -> MipsInstruction& {
        return this->instruction_stream.at(index); // safe access either way
    }

    void dasm(void);

};

std::string MipsTokenizer::cleanSource(std::string filename) {
    std::vector<char> char_stream;
    std::ifstream input_stream(filename);

    auto tmp_filename = "/tmp/djkrhdniwiwnsnbdcjdfneismwikd.asm";

    if(input_stream) {
        // get length of file:
        input_stream.seekg(0, input_stream.end);
        int length = input_stream.tellg();
        input_stream.seekg(0, input_stream.beg);

        char * buffer = new char [length];
        input_stream.read(buffer, length);

        // get rid of , ( )
        for(int i = 0; i < length; i++) {
            char c = buffer[i];
            if(c == ',' || c == '(' || c == ')')
                buffer[i] = ' ';
        }

        std::ofstream output_stream(tmp_filename);
        output_stream.write(buffer, length);
        output_stream.close();

        delete[] buffer;
    }

    input_stream.close();
    return tmp_filename;
}

MipsTokenizer::MipsTokenizer(std::string filename) {
    std::vector<std::string> string_stream;

    std::string new_filename = this->cleanSource(filename);
    std::ifstream input_stream(new_filename);
    std::string str;

    while((input_stream >> str))
        string_stream.push_back(str);

    MipsInstruction mi;

    // THANK YOU ALAN TURING!

    const int STATE_default     = 0;
    const int STATE_label       = 1;
    const int STATE_instruction = 2;

    const int STATE_std_math = 3;
    const int STATE_lw       = 4;
    const int STATE_sw       = 5;
    const int STATE_branch   = 6;
    const int STATE_imm_math = 7;

    int opcode_args = 0;
    int current_state = STATE_default;

    for(int i = 0; i < string_stream.size();) {
        std::string current_str = string_stream[i];

        #ifdef __DEBUG__
        std::cout << "TOKEN: " << current_str << std::endl;
        #endif

        switch(current_state) {
            case STATE_default:

                #ifdef __DEBUG__
                std::cout << "STATE_default\n";
                #endif

                if(current_str.back() == ':') {
                    current_state = STATE_label;
                } else {
                    current_state = STATE_instruction;
                }
                break;
            
            case STATE_label:

                #ifdef __DEBUG__
                std::cout << "STATE_label\n";
                #endif

                {
                    auto it = this->jump_offset_table.find(current_str);
                    if(it == this->jump_offset_table.end()) {

                        // entry doesnt exist, create it
                        //std::cout << "NEW JUMP ENTRY NEEDED\n    " << current_str << std::endl;
                        this->jump_offset_table[current_str] = this->jump_table.size();
                        this->jump_table.push_back(this->instruction_stream.size());

                    } else {

                        // jump offset already exists, populate it
                        //std::cout << "ENTRY ALREADY EXISTS...POPULATING" << std::endl;
                        int index = this->jump_offset_table[current_str];
                        this->jump_table[index] = this->instruction_stream.size();

                    }
                }
                
                current_state = STATE_default;
                i++; // advance op_Str
                break;

            case STATE_instruction:
                
                #ifdef __DEBUG__
                std::cout << "STATE_instruction\n";
                #endif 

                // figure out the opcode
                if(current_str == "add") {
                    mi.opcode = MIPS_inst::add;
                    current_state = STATE_std_math;

                } else if(current_str == "lw") {
                    mi.opcode = MIPS_inst::lw;
                    current_state = STATE_lw;

                } else if(current_str == "slt") {
                    mi.opcode = MIPS_inst::slt;
                    current_state = STATE_std_math;

                } else if(current_str == "beq") {
                    mi.opcode = MIPS_inst::beq;
                    current_state = STATE_branch;

                } else if(current_str == "addi") {
                    mi.opcode = MIPS_inst::addi;
                    current_state = STATE_imm_math;

                } else if(current_str == "sw") {
                    mi.opcode = MIPS_inst::sw;
                    current_state = STATE_sw;

                } else if(current_str == "bne") {
                    mi.opcode = MIPS_inst::bne;
                    current_state = STATE_branch;

                } else if(current_str == "halt") {
                    mi.opcode = MIPS_inst::halt;
                    // nothing else needed
                    this->instruction_stream.push_back(mi);

                } else {
                    throw std::runtime_error(std::string("UNKNOWN INSTRUCTION TOKEN: ") + current_str);
                }

                i++;
                break;

            case STATE_std_math:

                #ifdef __DEBUG__
                std::cout << "STATE_std_math\n";
                #endif 

                // three registers
                mi.argv[0] = registerOffset(string_stream[i+0]);
                mi.argv[1] = registerOffset(string_stream[i+1]);
                mi.argv[2] = registerOffset(string_stream[i+2]);

                current_state = STATE_default;
                i += 3; // skip the three operands
                this->instruction_stream.push_back(mi);
                break;

            case STATE_lw: // these two have the same types of args in the same order
            case STATE_sw:

                #ifdef __DEBUG__
                std::cout << "STATE_lw/STATE_sw\n";
                #endif

                mi.argv[0] = registerOffset(string_stream[i+0]);
                mi.argv[1] = std::stoi(string_stream[i+1]);
                mi.argv[2] = registerOffset(string_stream[i+2]);

                current_state = STATE_default;
                i += 3;
                this->instruction_stream.push_back(mi);
                break;

            case STATE_branch: // all branches share the same arg pattern
                #ifdef __DEBUG__
                std::cout << "STATE_branch\n";
                #endif

                mi.argv[0] = registerOffset(string_stream[i+0]);
                mi.argv[1] = registerOffset(string_stream[i+1]);

                current_str = string_stream[i+2];

                {
                    auto it = this->jump_offset_table.find(current_str + ":");
                    if(it == this->jump_offset_table.end()) {
                        // jump doesnt exist, create one
                        this->jump_offset_table[current_str + ":"] = this->jump_table.size();
                        this->jump_table.push_back(-1); // this will be evaluated/checked later
                        
                        mi.argv[2] = this->jump_table.size() - 1;
                    } else {
                        // jump already evaluated, place offset
                        mi.argv[2] = it->second;
                    }
                }

                current_state = STATE_default;
                i += 3;
                this->instruction_stream.push_back(mi);
                break;

            case STATE_imm_math:

                #ifdef __DEBUG__
                std::cout << "STATE_imm_math\n";
                #endif

                mi.argv[0] = registerOffset(string_stream[i+0]);
                mi.argv[1] = registerOffset(string_stream[i+1]);
                mi.argv[2] = std::stoi(string_stream[i+2]);

                current_state = STATE_default;
                i += 3;
                this->instruction_stream.push_back(mi);
                break;

            default:
                throw std::runtime_error(std::string("UNKNOWN TOKEN IN STREAM: ") + current_str);
        }

    }

    // last step is to finalize jump targets. they 
    // currently exist only as offsets into the jump table
    
    #ifdef __DEBUG__
    for(auto sp : jump_offset_table)
        std::cout << sp.first << "   " << sp.second << std::endl;
    #endif

    // first make sure every jump has a target
    for(auto i : jump_table) {
        if(i == -1) // error condition
            throw std::runtime_error("MipsTokenizer : ENTRY IN JUMP TABLE NOT EVALUATED");
    }

    // every jump has successfully been evaluated
    // time to place jump targets in the instruction stream
    for(auto& m : this->instruction_stream) {
        int op = m.opcode;
        if(op == MIPS_inst::beq || op == MIPS_inst::bne)
            m.argv[2] = this->jump_table[m.argv[2]];
    }
}

int MipsTokenizer::registerOffset(std::string reg_name) {
    auto iter = this->reg_name_to_int.find(reg_name);
    if(iter != reg_name_to_int.end())
        return iter->second; // register already exists, return the index

    // register doesnt exist yet, create it
    int new_register_index = this->int_to_reg_name.size();
    int_to_reg_name.push_back(reg_name);
    reg_name_to_int[reg_name] = new_register_index;
    return new_register_index;
}

int MipsTokenizer::strToOpcode(std::string op_str) {
    const std::map<std::string, int> op_table = {
        {"add",  MIPS_inst::add},
        {"lw",   MIPS_inst::lw},
        {"slt",  MIPS_inst::slt},
        {"sw",   MIPS_inst::sw},
        {"bne",  MIPS_inst::bne},
        {"addi", MIPS_inst::addi},
        {"halt", MIPS_inst::halt}
    };

    auto it = op_table.find(op_str);
    if(it != op_table.end())
        return it->second;

    throw std::runtime_error(std::string("MipsTokenizer::strToOpcode -> UNKNOWN OPCODE: ") + op_str);
}

void MipsTokenizer::dasm(void) {
    throw std::runtime_error("MipsTokenizer::dasm -> NOT YET IMPLEMENTED");
}

#ifdef __DEBUG__
#undef __DEBUG__
#endif // __DEBUG__ (obviously)

#endif // __JJC__MIPS__TOKENIZER__H__