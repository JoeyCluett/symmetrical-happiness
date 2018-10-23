#include <vector>

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
    InstructionQueue iq("sim.txt", registerFile);

    rstation_group_t add_sub_group(
        3,          // stations in this group
        {ADD, SUB}, // operations
        {2,   2});  // latencies for each operation above

    rstation_group_t mul_div_group(
        2, 
        {MUL, DIV},
        {10,  40});

    add_sub_group.reset();
    mul_div_group.reset();

    // starting state
    //cout << iq << endl << endl;
    //cout << registerFile << endl;

    // reservation station groups
    //cout << add_sub_group << endl << mul_div_group << endl;

    // tell the Tomasulo simulator about all of the hardware we have
    TomasuloUnit tu(
        {&add_sub_group, &mul_div_group},
        iq);

    cout << "\n============================================\n" << tu << endl;
    tu.simulate(2);
    cout << "\n============================================\n" << tu << endl;


    return 0;
}
