#include <iostream>
#include <vector>

#include <Bits.h>
#include <TwoBitCounter.h>
#include <HistoryPredictor.h>

using namespace std;

auto SN = TBC::StronglyNotTaken;
auto WN = TBC::WeaklyNotTaken;
auto WT = TBC::WeaklyTaken;
auto ST = TBC::StronglyTaken;

auto N = Branch::NotTaken;
auto T = Branch::Taken;

int main(int argc, char* argv[]) {

    // Problem 1


    // Problem 2
    cout << "\nProblem 2:\n\n";
    {
        HistoryPredictor hp(0, 1, {SN, SN}); // (start_state, state_bits, {counter_states})
        hp.runSimulation({N, N, N, T}, 12);
    }

    // Problem 3
    cout << "\nProblem 3:\n\n";
    {
        HistoryPredictor hp(0, 2, {SN, SN, SN, SN});
        hp.runSimulation({N, N, N, T}, 12, true);
    }

    // Problem 4
    cout << "\nProblem 4:\n\n";
    {
        HistoryPredictor hp(3, 2, {ST, ST, SN, SN});
        hp.runSimulation({N, N, N, T, N, T, T, T}, 30, true); // w/i 13 cycles
    }

    return 0;
}
