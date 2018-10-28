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
#include <string>

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
    if(argc != 4) {
        cout << "Usage:\n    " << argv[0] << " <cpu config file> <P1 simulator file> <'bmark'|'step'>\n";
        return 1;
    }

    ConfigGenerator cg(argv[1]);
    cg.useP1Format(argv[2]);
    cg.createCpuConfiguration();
    auto& tu = cg.tu();

    string runtime_option = argv[3];

    if(runtime_option == "bmark") {
        tu.benchmark();
        COUT << tu;
    }
    else if(runtime_option == "step") {

        COUT << tu;
        getchar();
        while(1) {
            tu.simulate(1); // simulate 1 clock cycle
            
            COUT << tu;
            getchar();
        }    
    }
    else {
        throw runtime_error("main.cpp -> expecting <'bmark'|'step'>, got " + runtime_option);
    }

    return 0;
}
