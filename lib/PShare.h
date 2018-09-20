#ifndef __JJC__PSHARE__PREDICTOR__H__
#define __JJC__PSHARE__PREDICTOR__H__

#include "TwoBitCounter.h"
#include "HistoryPredictor.h"
#include "BranchPredictor.h" // NotTaken, Taken
#include "Bits.h"
#include <jjc_macros.h>

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

    u_int64_t getOutput(void) {
        return this->shift_history;
    }

    int getNumBits(void) {
        return this->num_bits;
    }

    friend std::ostream& operator<<(std::ostream& os, PhtEntry& pe) {
        os << Bits::bitPattern<uint64_t>(pe.shift_history, pe.num_bits);
        return os;
    }

};

class PsharePredictor : public SharePredictor {
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
        this->pc_bit_mask = Bits::chunk(bits_as_pht_offset, 2); // 2 is constant because the first two bits are always zero
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

    void initPrivateHistoryTable(int entries, int bits_per_entry, long int start_state = 0L) {
        for(int i = 0; i < entries; i++)
            this->private_history_table.push_back(PhtEntry(bits_per_entry, start_state));
    }

    Branch getPrediction(addr_t address) override {
        int index = (address & pc_bit_mask) >> 2;
        this->last_pht_index = index; // need this for update

        // two pieces get xor'ed
        index = (index ^ (int)private_history_table.at(index).getOutput()) & this->bht_index_mask;
        this->last_bht_index = index; // ...

        this->last_predicton = this->branch_history_table.at(index).getPrediction();
        return this->last_predicton;
    }

    bool update(Branch branch) override {
        this->branch_history_table.at(last_bht_index).update(branch);
        this->private_history_table.at(last_pht_index).update(branch);

        return (branch == this->last_predicton);
    }

    bool simulate(BranchOccasion& bo) override {
        //return false; // ALWAYS FAIL!!

        Branch b = this->getPrediction(bo.address);
        return this->update(bo.branch_result);
    }

    friend std::ostream& operator<<(std::ostream& os, PsharePredictor& psp) {
        const char* label = "PHT";
        
        auto& pht = psp.private_history_table;
        auto& bht = psp.branch_history_table;

        os << "     " << label << SPACES(pht[0].getNumBits() + 1) << "BHT\n";

        int pht_len = psp.private_history_table.size();
        int bht_len = psp.branch_history_table.size();

        int max_len = pht_len;
        if(bht_len > max_len)
            max_len = bht_len;

        for(int i = 0; i < max_len; i++) {
            std::string n = std::to_string(i);
            os << SPACES(2-n.size()) << n << " : ";

            if(i < pht.size())
                os << pht[i];
            else
                os << SPACES(pht[i].getNumBits());

            os << "    ";

            if(i < bht.size())
                os << bht[i] << std::endl;
            else
                os << std::endl;
        }

        return os;
    }

};

#endif // __JJC__PSHARE__PREDICTOR__H__