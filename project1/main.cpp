#include <vector>
#include <stdio.h>
#include <iostream>

// custom libs (tomasulo folder, the compiler directs you to this)
#include <Constants.h>
#include <RegisterFile.h>
#include <InstructionQueue.h>
#include <ReservationStation.h>
#include <TomasuloUnit.h>

/*
    A completely modular and configurable 
    Tomasulos Algorithm simulator

    Every instance must have:
        a.) a register file with well-defined number of registers.
            this is something that you must know ahead of time, 
            though I could write a configuration generator

        b.) a single instruction queue. the queue is realized as 
            a std::vector and the InstructionEntry class takes 
            care of tracking all information needed to properly 
            execute it

            currently, the instruction queue is responsible for 
            parsing the input file and creating the proper 
            instruction sequences

        c.) One or more reservation station groups. a group of 
            stations is associated with an execution unit that 
            is managed internally (dont worry about it)

            when a ReservationStationEntry is instantiated, a 
            reference to it is stored in an internal queue that 
            maintains its order across multiple method calls and 
            is accessed like a global pointer table. each entry, 
            therefore, only needs to maintain in index into this table

        d.) an instance of TomasuloUnit class. this takes care of 
            passing data between all of the components given. it is 
            written in a very general-purpose way so you can have 
            any hardware configuration you'd like

    This simulator does not currently support a re-order buffer
    getchar() is used to pause the simulator between simulated clock cycles
*/

using namespace std;

int main(int argc, char* argv[]) {
    // register file has to be instantiated separately
    reg_file_t registerFile(8); // 8 entries

    // instructions are loaded here prior to execution
    InstructionQueue iq(
        "sim2.txt", 
        registerFile);

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
        {&add_sub_group, &mul_div_group}, // reservation stations (pointers)
        iq,                               // instruction queue
        registerFile);                    // register file

    const char* sep = "\n============================================\n\n";

    //CLEAR_SCREEN;
    cout << sep << tu << endl;
    getchar();

    for(int i = 0; i < 20; i++) {
        cout << sep;
        tu.simulate(1); // simulate 1 clock cycle
        cout << tu << endl;

        getchar();
    }    

    return 0;
}
