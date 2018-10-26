#ifndef __JJC__CONFIG__GENERATOR__H__
#define __JJC__CONFIG__GENERATOR__H__

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <iostream>

// custom libs
#include "Constants.h"
#include "TomasuloUnit.h"
#include "ReservationStation.h"
#include "RegisterFile.h"

class ConfigGenerator {
private:
    int number_of_registers = -1;
    std::vector<int> registers;

    // cant create simulation without an instruction queue
    bool has_instructions = false;

    // used to hold modules for simulation
    TomasuloUnit* tu_ptr = NULL;

public:
    // config file uses custom file format specification
    ConfigGenerator(const std::string& filename);

    // just a simple MIPS32 assembly file. currently much 
    // more restricted than anything in the asm directory
    void useAsmFile(const std::string& filename);

    // performs operations to internally synthesize 
    // tomasulo hardware (in software of course)
    void createCpuConfiguration(void);

    TomasuloUnit& tu(void) {
        return *tu_ptr;
    }
};

ConfigGenerator::ConfigGenerator(const std::string& filename) {
    const int STATE_start    = 0;
    const int STATE_default  = 1;
    const int STATE_num_regs = 2;
    const int STATE_reg_vals = 3;
    const int STATE_rstation = 4;

    int current_state = STATE_start;

    const int STATE_rstation_default = 0;
    const int STATE_rstation_entries = 1;
    const int STATE_rstation_ops     = 2;
    const int STATE_rstation_latency = 3;

    int current_rstation_state = STATE_rstation_default;

    std::string current_token = "CONFIG_END";
    std::ifstream input_file(filename, std::ios::in);

    std::cout << "Reading configuration file...";

    // for configuring reservation station entries
    int reservation_station_entries = -1;
    std::vector<int> rstation_operations;
    std::vector<int> rstation_latencies;

    do {
        input_file >> current_token;

        switch(current_state) {
            case STATE_start:
                if(current_token != "CONFIG_START") {
                    throw std::runtime_error(
                        "ConfigGenerator, STATE_start -> expecting CONFIG_START (got " 
                        + current_token + ")");
                } else {
                    current_state = STATE_default;
                }
                break;
            case STATE_default:
                if(current_token == "_num_registers") {
                    current_state = STATE_num_regs;
                }
                else if(current_token == "_register_start_values") {
                    current_state = STATE_reg_vals;
                }
                else if(current_token == "_reservation_station_group_start") {
                    current_state = STATE_rstation;
                }
                else if(current_token == "CONFIG_END") {
                    std::cout << "DONE\n";
                }
                break;
            case STATE_num_regs:
                this->number_of_registers = std::stoi(current_token);
                current_state = STATE_default;
                break;
            case STATE_reg_vals:
                if(this->number_of_registers < 0)
                    throw std::runtime_error("ConfigGenerator, STATE_reg_vals -> specify the number of registers before their starting values");
                if(this->registers.size() < this->number_of_registers) {
                    this->registers.push_back(std::stoi(current_token));
                } else {
                    if(current_token != "_end") {
                        throw std::runtime_error("ConfigGenerator, STATE_reg_vals -> expecting '_end' token after register values");
                    } else {
                        current_state = STATE_default;
                    }
                }
                break;
            case STATE_rstation:
                switch(current_rstation_state) {
                    case STATE_rstation_default:
                        if(current_token == "_entries") {

                        }
                        else if(current_token == "_operations") {

                        }
                        else if(current_token == "_latency") {

                        }
                        else if(current_token == "_reservation_station_group_end") {
                            current_state = STATE_default;
                            
                            if(rstation_operations.size() != rstation_latencies.size()) {
                                
                            }
                        }
                        else {
                            throw std::runtime_error(
                                "ConfigGenerator, STATE_rtstaion -> unknown token: " 
                                + current_token);
                        }
                        break;
                    case STATE_rstation_entries:

                        break;
                    case STATE_rstation_ops:
                        break;
                    case STATE_rstation_latency:
                        break;
                    default:
                        throw std::runtime_error(
                            "ConfigGenerator, STATE_rstation -> unknown substate"
                        );
                }
            default:
                throw std::runtime_error("ConfigGenerator::ConfigGenerator -> unknown outer state");
        }

    } while(current_token != "CONFIG_END");

}

#endif // __JJC__CONFIG__GENERATOR__H__