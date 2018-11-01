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
    // register configuration
    int number_registers = -1;
    std::vector<int> register_start_values;

    // reservation stations go here before going into the TomasuloUnit
    std::vector<ReservationStationGroup*> reservation_station_storage;

    // cant create simulation without an instruction queue
    bool has_instructions = false;
    InstructionQueue* iq_ptr = NULL;

    // used to hold modules for simulation
    RegisterFile* reg_file = NULL;
    TomasuloUnit* tu_ptr = NULL;

    void parse_xml_config_file(const std::string& filename);

public:
    // config file uses custom file format specification
    ConfigGenerator(const std::string& filename, const int parse_file_type = CONFIG_TYPE_CUSTOM);

    // just a simple MIPS32 assembly file. currently much 
    // more restricted than anything in the asm directory
    void useAsmFile(const std::string& filename);

    // use the format given for project1
    void useP1Format(const std::string& filename) {
        this->iq_ptr = new InstructionQueue(filename, *this->reg_file);
        this->has_instructions = true;
    }

    // performs operations to internally synthesize 
    // tomasulo hardware (in software of course)
    void createCpuConfiguration(void);

    TomasuloUnit& tu(void) {
        return *tu_ptr;
    }
};

void ConfigGenerator::createCpuConfiguration(void) {
    if(this->has_instructions == false) {
        throw std::runtime_error(
            "ConfigGenerator::createCpuConfiguration -> no instruction stream present");
    }

    tu_ptr = new TomasuloUnit(
            this->reservation_station_storage,
            *this->iq_ptr,
            *this->reg_file);
}

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

    // for configuring reservation station entries.
    // using temporaries here because you can have 
    // more than one group of reservation station entries
    int reservation_station_entries = -1;
    std::vector<int> rstation_operations;
    std::vector<int> rstation_latencies;

    do {
        input_file >> current_token;

        switch(current_state) {
            case STATE_start:
                //std::cout << "  STATE_start\n";

                if(current_token != "CONFIG_START" && current_token != "ConfigStart") {
                    throw std::runtime_error(
                        "ConfigGenerator, STATE_start -> expecting CONFIG_START (got " 
                        + current_token + ")");
                } else {
                    current_state = STATE_default;
                }
                break;
            case STATE_default:
                //std::cout << "  STATE_default\n";

                if(current_token == "_num_registers" ||
                        current_token == "NumRegisters") {
                    current_state = STATE_num_regs;
                }
                else if(current_token == "_register_start_values" ||
                        current_token == "RegisterStartValues") {
                    current_state = STATE_reg_vals;
                }
                else if(current_token == "_reservation_station_group_start" ||
                        current_token == "ReservationStationGroupStart") {
                    current_state = STATE_rstation;
                }
                else if(current_token == "CONFIG_END" ||
                        current_token == "ConfigEnd") {
                    // verify a few pieces of information
                    if(this->number_registers != this->register_start_values.size())
                        throw std::runtime_error(
                            "ConfigGenerator -> number of register start values must equal number of registers requested"
                        );
                    
                    // create a register file
                    this->reg_file = new RegisterFile(this->number_registers);
 
                    std::cout << "DONE\n";
                }
                break;
            case STATE_num_regs:
                //std::cout << "  STATE_num_regs\n";

                if(this->number_registers != -1) {
                    throw std::runtime_error(
                        "ConfigGenerator, STATE_num_regs -> only specify the number of registers once"
                    );
                }
                this->number_registers = std::stoi(current_token);
                current_state = STATE_default;
                break;
            case STATE_reg_vals:
                //std::cout << "  STATE_reg_vals\n";

                if(current_token != "_end" && current_token != "End") {
                    this->register_start_values.push_back(std::stoi(current_token));
                } else {
                    current_state = STATE_default;
                }
                break;
            case STATE_rstation:
                //std::cout << "  STATE_rstation\n";

                switch(current_rstation_state) {
                    case STATE_rstation_default:
                        //std::cout << "    STATE_rstation_default\n";

                        if(current_token == "_entries" ||
                                current_token == "Entries") {
                            current_rstation_state = STATE_rstation_entries;
                        }
                        else if(current_token == "_operations" ||
                                current_token == "Operations") {
                            current_rstation_state = STATE_rstation_ops;
                        }
                        else if(current_token == "_latency" ||
                                current_token == "Latency") {
                            current_rstation_state = STATE_rstation_latency;
                        }
                        else if(current_token == "_reservation_station_group_end" ||
                                current_token == "ReservationStationGroupEnd") {

                            if(rstation_operations.size() != rstation_latencies.size()) {
                                throw std::runtime_error(
                                    "ConfigGenerator -> station operations and station latencies need to have the same number of entries"
                                );
                            }

                            if(reservation_station_entries == -1) {
                                throw std::runtime_error(
                                    "ConfigGenerator, rstation_group_end -> need to specify number of entries"
                                );
                            }

                            // create a reservation station group
                            auto rstation_ptr = new ReservationStationGroup(
                                    reservation_station_entries,
                                    rstation_operations,
                                    rstation_latencies);
                            rstation_ptr->reset();

                            this->reservation_station_storage
                                    .push_back(rstation_ptr);

                            // reset temp storage for next station group
                            reservation_station_entries = -1;
                            rstation_operations.clear();
                            rstation_latencies.clear();

                            // set proper state for next iteration
                            current_state = STATE_default;
                        }
                        else {
                            throw std::runtime_error(
                                "ConfigGenerator, STATE_rtstaion_default -> unknown token: " 
                                + current_token);
                        }
                        break;
                    case STATE_rstation_entries:
                        //std::cout << "    STATE_rstation_entries\n";

                        if(reservation_station_entries != -1) {
                            throw std::runtime_error(
                                "ConfigGenerator, STATE_rstation_entries -> set the number of entries once per station group"
                            );
                        }

                        reservation_station_entries = std::stoi(current_token);
                        current_rstation_state = STATE_rstation_default;
                        break;
                    case STATE_rstation_ops:
                        //std::cout << "    STATE_rstation_ops\n";

                        if(current_token == "_end" ||
                                current_token == "End") {
                            current_rstation_state = STATE_rstation_default;
                        } else {
                            if(current_token == "ADD") {
                                rstation_operations.push_back(ADD);
                            }
                            else if(current_token == "SUB") {
                                rstation_operations.push_back(SUB);
                            }
                            else if(current_token == "MUL") {
                                rstation_operations.push_back(MUL);
                            }
                            else if(current_token == "DIV") {
                                rstation_operations.push_back(DIV);
                            }
                            else {
                                throw std::runtime_error(
                                    "ConfigGenerator, STATE_rstation_ops -> UNKNOWN OPERATION: "
                                    + current_token
                                );
                            }
                        }
                        break;
                    case STATE_rstation_latency:
                        //std::cout << "    STATE_rstation_latency\n";

                        if(current_token == "_end" ||
                                current_token == "End") {
                            current_rstation_state = STATE_rstation_default;
                        } else {
                            rstation_latencies.push_back(std::stoi(current_token));
                        }
                        break;
                    default:
                        throw std::runtime_error(
                            "ConfigGenerator, STATE_rstation -> unknown substate"
                        );
                }
                break;
            default:
                throw std::runtime_error("ConfigGenerator::ConfigGenerator -> unknown outer state");
        }

    } while(current_token != "CONFIG_END" && current_token != "ConfigEnd");

}

#endif // __JJC__CONFIG__GENERATOR__H__