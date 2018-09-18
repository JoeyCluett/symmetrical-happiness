#include <iostream>
#include <vector>

#include <Bits.h>
#include <TwoBitCounter.h>
#include <HistoryPredictor.h>
#include <PShare.h>

#include <MIPS_Tokenizer.h>
#include <MIPS_Constants.h>
#include <MIPS_Runtime.h>

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

    // Problem 6
    cout << "\nProblem 6:\n\n";
    {
        MipsTokenizer mt("../asm/hw2-pb6.asm");

        // executes MIPS32 tokens from the MIPS ASM tokenizer
        MipsRuntime mr;

        // setup the runtime
        mr.setStartInstructionAddress(0x800007C0); // seldom used apart from branch history

        mr.setStartDataAddress(0x00000000);
        mr.pokeMem(0x00000000, {21, 18, 42, 7, 9, 31});
        mr.reg(MIPS_reg::s[0]) = 0; // register points to the array in memory

        // we will want to track various pieces of machinery in the simulator
        mr.setTrackBranches(true);     // keep branching history (12 bytes/branch occurance, so be careful)
        mr.setProgramLinearity(false); // this will be most programs

        // execute the token stream for the given number of cycles
        mr.execute(mt, 40);

        cout << "Branch history: \n";
        for(auto& bh : mr.getBranchHistory())
            cout << bh << endl;
    }

    // Problem 7
    cout << "\nProblem 7:\n\n";
    {
        PsharePredictor psp(4); // 4 bits in the address used to calculate offset into PHT
        psp.initPrivateHistoryTable(16, 2); // 16 PHT entries, 2 bits each, start at 0
        psp.initBranchHistoryTable({SN, SN, SN, SN}); // BHT entries are assumed to be two-bit counters

        MipsTokenizer mt("../asm/hw2-pb6.asm");
        MipsRuntime mr;

        mr.setStartInstructionAddress(0x800007C0);
        mr.pokeMem(0x00000000, {21, 18, 42, 7, 9, 31});
        mr.reg(MIPS_reg::s[0]) = 0;

        // let simulator do its work for a few instructions
        
    }

    return 0;
}
