#include <vector>

// custom libs (tomasulo folder)
#include <RegisterFile.h>
#include <InstructionQueue.h>

using namespace std;

int main(int argc, char* argv[]) {
    // opcode stuff
    const int add = 0;
    const int sub = 1;
    const int mul = 2;
    const int _div = 3; // div is used elsewhere in the STL lib

    // register file has to be instantiated seperately
    reg_file_t registerFile(8); // 8 entries
    InstructionQueue iq("sim.txt", registerFile);

    // starting state
    cout << iq << endl << endl;
    cout << registerFile << endl;

    

    return 0;
}
