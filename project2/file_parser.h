#pragma once

#include <string>
#include <fstream>
#include <string>

#include <instruction_queue.h>
#include <register_file.h>

class file_parser_t {
public:

    int num_instructions = 0;
    int num_cycles = 0;

    file_parser_t(std::string filename, instruction_queue_t& iq, register_file_t& rf) {
        auto get_instruction = [&](std::ifstream& ifs) {
            instruction_entry_t ie;

            int operation;
            ifs >> operation;
            
            switch(operation) {
                case 0:
                    ie.op = operation_t::_add; 
                    break;
                case 1:
                    ie.op = operation_t::_subtract; 
                    break;
                case 2:
                    ie.op = operation_t::_multiply; 
                    break;
                case 3:
                    ie.op = operation_t::_divide; 
                    break;
                default:
                    throw std::runtime_error("Unknown operation is sim file");
                    break;
            }

            ifs >> ie.dest >> ie.src1 >> ie.src2;
            iq.stream.push_back(ie);
        };

        std::ifstream ifs(filename);
        ifs >> this->num_instructions;
        ifs >> this->num_cycles;

        for(int i = 0; i < this->num_instructions; i++)
            get_instruction(ifs);

        for(int i = 0; i < 8; i++)
            ifs >> rf.value(i);
    }

};