/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#include <vector>
#include <stdio.h>
#include <iostream>

// custom libs (tomasulo folder, the compiler directs you to this)
#include <Constants.h>
#include <RegisterFile.h>
#include <InstructionQueue.h>
#include <ReservationStation.h>
#include <TomasuloUnit.h>

// moves hardware config from main to separate file
#include <ConfigGenerator.h>

using namespace std;

int main(int argc, char* argv[]) {

/*
    // register file has to be instantiated separately
    reg_file_t registerFile(8); // 8 entries

    // instructions are loaded here prior to execution
    InstructionQueue iq(
        "sim2.txt", 
        registerFile);

    rstation_group_t add_sub_group(
        //3,          // stations in this group
        4,
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

    //const char* sep = "\n============================================\n\n";

*/

    ConfigGenerator cg("progs/example.config");
    cg.useP1Format("sim2.txt");
    cg.createCpuConfiguration();
    auto& tu = cg.tu();

    COUT << tu;
    getchar();

    while(1) {
        tu.simulate(1); // simulate 1 clock cycle
        
        COUT << tu;
        getchar();
    }    

    return 0;
}
