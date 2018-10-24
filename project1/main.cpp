#include <vector>
#include <stdio.h>
#include <iostream>

// custom libs (tomasulo folder, the compiler directs you to this)
#include <Constants.h>
#include <RegisterFile.h>
#include <InstructionQueue.h>
#include <ReservationStation.h>
#include <TomasuloUnit.h>

using namespace std;

int main(int argc, char* argv[]) {
    // register file has to be instantiated seperately
    reg_file_t registerFile(8); // 8 entries

    // instructions are loaded here prior to execution
    InstructionQueue iq("sim2.txt", registerFile);

    rstation_group_t add_sub_group(
        3,          // stations in this group
        {ADD, SUB}, // operations
        {2,   2});  // latencies for each operation above

    rstation_group_t mul_div_group(
        2, 
        {MUL, DIV},
        {10,  40});

    // start with no operations queued
    add_sub_group.reset();
    mul_div_group.reset();

    // tell the Tomasulo simulator about all of the hardware we have
    TomasuloUnit tu(
        {&add_sub_group, &mul_div_group}, // reservation stations
        iq,                               // instruction queue
        registerFile);                    // register file

    const char* sep = "\n============================================\n\n";

    //CLEAR_SCREEN;
    cout << sep << tu << endl;
    getchar();

    for(int i = 0; i < 15; i++) {
        cout << sep;
        tu.simulate(1); // simulate 1 clock cycle
        cout << tu << endl;

        getchar();
    }    

    return 0;
}
