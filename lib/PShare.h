#ifndef __JJC__PSHARE__PREDICTOR__H__
#define __JJC__PSHARE__PREDICTOR__H__

#include "TwoBitCounter.h"
#include "HistoryPredictor.h"
#include "BranchOccasion.h"
#include "BranchPredictor.h" // NotTaken, Taken
#include "Bits.h"

#include <vector>
#include <iostream>

class PhtEntry {
private:
    u_int64_t shift_history;
    u_int64_t shift_bit;
    int num_bits;

public:
    PhtEntry(int bits, u_int64_t starting_state = 0L) {
        shift_bit = 1L;
        shift_bit <<= (bits - 1); // prep the bit that gets shifted in on the left

        this->shift_history = starting_state;
        this->num_bits = bits;
    }

    void update(Branch branch) {
        shift_history >>= 1;
        if(branch == Branch::Taken)
            shift_history |= shift_bit; // shift in leftmost bit
    }

    u_int64_t getIndex(void) {
        return this->shift_history;
    }

};

class PsharePredictor {
private:
    std::vector<TwoBitCounter> branch_history_table;
    std::vector<PhtEntry>      private_history_table;
    int pc_bit_mask = 0;
    
    int last_pht_index    = -1;
    int last_bht_index    = -1;
    Branch last_predicton = Branch::Unknown;

    int bht_index_mask = -1;

public:
    PsharePredictor(int bits_as_pht_offset) {
        this->pc_bit_mask = Bits::chunk(bits_as_pht_offset, 2);
    }

    void initBranchHistoryTable(std::vector<TBC> bht_states) {
        this->branch_history_table.clear();
        for(auto n : bht_states)
            this->branch_history_table.push_back(TwoBitCounter(n));
    }

    void initBranchHistoryTable(int n, TBC start_state) {
        this->branch_history_table.clear();
        for(int i = 0; i < n; i++)
            this->branch_history_table.push_back(TwoBitCounter(start_state));
    }

    void setBhtIndexMask(int bits) {
        this->bht_index_mask = Bits::chunk(bits, 0);
    }

    void initPrivateHistoryTable(int pht_bits, std::vector<u_int64_t> states) {
        this->private_history_table.clear();
        for(auto l : states)
            this->private_history_table.push_back(PhtEntry(pht_bits, l));
    }

    void initPrivateHistoryTable(int pht_states, u_int64_t state_all = 0L) {
        private_history_table.clear();

        int tmp = pht_states;

        int state_bits = 1;
        while(tmp > 1) {
            state_bits ++;
            tmp /= 2;
        }

        for(int i = 0; i < pht_states; i++)
            this->private_history_table.push_back(PhtEntry(state_bits, state_all));
    }

    Branch getPrediction(addr_t address) {
        int index = (address & pc_bit_mask) >> 2;
        this->last_pht_index = index; // need this for update

        // two pieces get xor'ed
        index = (index ^ (int)private_history_table.at(index).getIndex()) & this->bht_index_mask;
        this->last_bht_index = index; // ...

        this->last_predicton = this->branch_history_table.at(index).getPrediction();
        return this->last_predicton;
    }

    bool update(Branch branch) {
        this->branch_history_table.at(last_bht_index).update(branch);
        this->private_history_table.at(last_pht_index).update(branch);

        return (branch == this->last_predicton);
    }

    friend std::ostream& operator<<(std::ostream& os, PsharePredictor& psp) {
        os << std::endl;
        
        

        return os;
    }

};

#endif // __JJC__PSHARE__PREDICTOR__H__