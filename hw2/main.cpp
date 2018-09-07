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

    // Problem 2
    cout << "Problem 2:\n\n";
    HistoryPredictor hp_2(0, 1, {SN, SN});
    hp_2.runSimulation({T, T}, 10);



    return 0;
}
