#ifndef __JJC__MIPS__TOKENIZER__H__
#define __JJC__MIPS__TOKENIZER__H__

#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include "MIPS_Constants.h"

class MipsInstruction {
public:
    int opcode;
    int argv[3];
};

class MipsTokenizer {
private:
    std::vector<MipsInstruction> instruction_stream;
    std::vector<int> jump_table;
    std::map<std::string, int> jump_offset_table;

    auto cleanSource(std::string filename) -> std::string; // returns new filename
    auto finalizeJumpTargets(void) -> void; // turns jump offsets into targets

public:
    MipsTokenizer(std::string filename);

    int size(void) {
        return this->instruction_stream.size();
    }

    int strToOpcode(std::string op_str);
    int strToRegOffset(std::string reg_str);

    auto operator[](int index) -> MipsInstruction& {
        return this->instruction_stream.at(index);
    }

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

        switch(current_state) {
            case STATE_default:

                if(current_str.back() == ':') {
                    current_state = STATE_label;
                } else {
                    current_state = STATE_instruction;
                }
                break;
            
            case STATE_label:

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

                } else {
                    throw std::runtime_error(std::string("UNKNOWN INSTRUCTION TOKEN: ") + current_str);
                }

                i++;
                break;

            case STATE_std_math:

                // three registers
                mi.argv[0] = strToRegOffset(string_stream[i+0]);
                mi.argv[1] = strToRegOffset(string_stream[i+1]);
                mi.argv[2] = strToRegOffset(string_stream[i+2]);

                current_state = STATE_default;
                i += 3; // skip the three operands
                break;

            case STATE_lw: // these two have the same types of args in the same order
            case STATE_sw:

                mi.argv[0] = strToRegOffset(string_stream[i+0]);
                mi.argv[1] = std::stoi(string_stream[i+1]);
                mi.argv[2] = strToRegOffset(string_stream[i+2]);

                current_state = STATE_default;
                i += 3;
                break;

            case STATE_branch: // all branches share the same arg pattern
                //std::cout << "BRANCH\n";

                mi.argv[0] = strToRegOffset(string_stream[i+0]);
                mi.argv[1] = strToRegOffset(string_stream[i+1]);

                {
                    auto it = this->jump_offset_table.find(current_str + ":");
                    if(it == this->jump_offset_table.end()) {
                        // jump doesnt exist, create one
                        this->jump_offset_table[current_str] = this->jump_table.size();
                        this->jump_table.push_back(-1); // this will be evaluated/checked later
                    } else {
                        // jump already evaluated, place offset
                        mi.argv[2] = it->second;
                    }
                }

                current_state = STATE_default;
                i += 3;
                break;

            case STATE_imm_math:

                mi.argv[0] = strToRegOffset(string_stream[i+0]);
                mi.argv[1] = strToRegOffset(string_stream[i+1]);
                mi.argv[2] = std::stoi(string_stream[i+2]);

                current_state = STATE_default;
                i += 3;
                break;

            default:
                throw std::runtime_error(std::string("UNKNOWN TOKEN IN STREAM: ") + current_str);
        }
    }

    // final step is to finalize jump targets. they 
    // currently exist only as offsets into the jump table

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


int MipsTokenizer::strToRegOffset(std::string reg_str) {
    if(reg_str[0] != '$')
        throw std::runtime_error(std::string("UNKNOWN REGISTER STRING: ") + reg_str);

    if(reg_str == "$zero" || reg_str == "$0")
        return 0;

    if(reg_str == "$at")
        return 1;

    if(reg_str == "$gp")
        return 28;

    if(reg_str == "$sp")
        return 29;

    if(reg_str == "$fp")
        return 30;

    if(reg_str == "$ra")
        return 31;

    if(reg_str[1] == 't') {
        
        int tmp = std::stoi(reg_str.c_str() + 2);
        if(tmp <= 7 && tmp >= 0)
            return tmp + 8;
        else if(tmp == 8 || tmp == 9)
            return tmp + 16;

    } else if(reg_str[1] == 's') {

        int tmp = std::stoi(reg_str.c_str() + 2);
        if(tmp <= 7 && tmp >= 0)
            return tmp + 16;

    } else if(reg_str[1] == 'a') {

        int tmp = std::stoi(reg_str.c_str() + 2);
        if(tmp <= 3 && tmp >= 0)
            return tmp + 4;

    } else if(reg_str[1] == 'v') {

        int tmp = std::stoi(reg_str.c_str() + 2);
        if(tmp == 0 || tmp == 1)
            return tmp + 2;

    } else if(reg_str[1] == 'k') {

        int tmp = std::stoi(reg_str.c_str() + 2);
        if(tmp == 0 || tmp == 1)
            return tmp + 26;

    } else if(reg_str[1] == 'f') {

        return std::stoi(reg_str.c_str() + 2) + 32; // mips32 has 32 integer registers and 32 fp registers

    }

    throw std::runtime_error(std::string("ERROR IN REGISTER FILE: ") + reg_str);
}

int MipsTokenizer::strToOpcode(std::string op_str) {
    const std::map<std::string, int> op_table = {
        {"add", MIPS_inst::add},
        {"lw", MIPS_inst::lw},
        {"slt", MIPS_inst::slt},
        {"sw", MIPS_inst::sw},
        {"bne", MIPS_inst::bne},
        {"addi", MIPS_inst::addi}
    };

    auto it = op_table.find(op_str);
    if(it != op_table.end())
        return it->second;

    throw std::runtime_error(std::string("UNKNOWN OPCODE: ") + op_str);
}

#endif // __JJC__MIPS__TOKENIZER__H__