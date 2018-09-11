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
    int argv[4];
};

class MipsTokenizer {
private:
    std::vector<MipsInstruction> instruction_stream;
    std::vector<int> jump_table;
    std::map<std::string, int> jump_offset_table;

    auto cleanSource(std::string filename) -> std::string;

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

    char buf[1024];

    int r = 0;
    do {
        
    } while(r > 0);

    return tmp_filename;

}

MipsTokenizer::MipsTokenizer(std::string filename) {
    std::vector<std::string> string_stream;
    std::ifstream input_stream(filename);
    std::string str;

    while((input_stream >> str))
        string_stream.push_back(str);

    // THANK YOU ALAN TURING!

    const int STATE_default     = 0;
    const int STATE_label       = 1;
    const int STATE_instruction = 2;

    const int STATE_std_math = 3;
    const int STATE_lw       = 4;
    const int STATE_sw       = 5;
    const int STATE_branch   = 6;

    int opcode_args = 0;
    int current_state = STATE_default;

    for(int i = 0; i < string_stream.size();) {
        string current_str = string_stream[i];

        switch(current_state) {
            case STATE_default:
                if(current_str.back() == ':') {
                    current_state = STATE_label;
                } else {
                    current_state = STATE_instruction;
                }
                break;
            
            case STATE_label:

                i++; // advance op_Str
                break;

            case STATE_instruction:
                

            default:

        }
    }
}


int MipsTokenizer::strToRegOffset(std::string reg_str) {
    if(reg_str[0] != '$')
        throw std::runtime_error(std::string("UNKNOWN REGISTER STRING: ") + reg_str);

    if(reg_str == "$zero")
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