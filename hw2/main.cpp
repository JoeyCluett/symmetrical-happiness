#include <iostream>

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

    vector<Branch> branch_pattern = {N, N, N, T};

    HistoryPredictor hp(0, 3, {SN, SN, SN, SN, SN, SN, SN, SN}); // 3-bit predictor

    hp.runSimulation(branch_pattern, 20);

    /*for(int i = 0; i < 24; i++) {
        cout << hp;
        auto pred   = hp.getPrediction();
        auto actual = branch_pattern[i % 4];

        cout << "    " << pred << "    " << actual << "    ";

        hp.update(pred, actual);

        cout << PRINT_BOOL(pred == actual) << endl;
    }*/

    return 0;
}