#include <vector>

// custom libs (tomasulo folder)
#include <Constants.h>
#include <RegisterFile.h>
#include <InstructionQueue.h>
#include <ReservationStation.h>

using namespace std;

int main(int argc, char* argv[]) {
    // register file has to be instantiated seperately
    reg_file_t registerFile(8); // 8 entries

    // instructions are loaded here prior to execution
    InstructionQueue iq("sim.txt", registerFile);

    rstation_group_t add_sub_group(
        3, 
        {ADD, SUB},
        {2,   2});

    rstation_group_t mul_div_group(
        2, 
        {MUL, DIV},
        {10,  40});

    // starting state
    cout << iq << endl << endl;
    cout << registerFile << endl;

    

    return 0;
}
