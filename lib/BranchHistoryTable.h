#ifndef __JJC__BRANCH__HISTORY__TABLE__H__
#define __JJC__BRANCH__HISTORY__TABLE__H__

#include <vector>
#include "Bits.h"
#include "BranchPredictor.h"

class BranchHistoryTable_1Bit : public PC_Predictor {
private:
    std::vector<Branch> lookup_table;
    int lookup_mask = 0;

    int    prev_lookup = -1;
    Branch prev_prediction = Branch::Unknown;

public:
    BranchHistoryTable_1Bit(int num_entries, int mask_width, Branch branch) {
        for(int i = 0; i < num_entries; i++)
            lookup_table.push_back(branch);
        lookup_mask = Bits::chunk(mask_width, 2);
    }

    Branch getPrediction(addr_t address) override {
        int index = ((address & lookup_mask) >> 2);

        prev_lookup = index;
        prev_prediction = lookup_table.at(index);

        return prev_prediction;
    }

    bool update(Branch branch) override {
        lookup_table.at(prev_lookup) = branch;
        return (branch == prev_prediction);
    }

};

#endif // __JJC__BRANCH__HISTORY__TABLE__H__