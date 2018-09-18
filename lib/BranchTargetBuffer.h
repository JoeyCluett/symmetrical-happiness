#ifndef __JJC__BRANCH__TARGET__BUFFER__H__
#define __JJC__BRANCH__TARGET__BUFFER__H__

#include <vector>
#include "BranchPredictor.h"
#include "Bits.h"

class BranchTargetBuffer {
private:
    std::vector<addr_t> target_vec;
    addr_t index_mask = 0;

public:
    BranchTargetBuffer(int index_bits, int entries, addr_t starting_state = 0L) {
        for(int i = 0; i < entries; i++)
            target_vec.push_back(starting_state);

        index_mask = Bits::chunk<addr_t>(index_bits, 2);
    }

    BranchTargetBuffer(int index_bits, std::vector<addr_t> starting_states) {
        target_vec = starting_states;
        index_mask = Bits::chunk<addr_t>(index_bits, 2);
    }

    addr_t target(addr_t index) {
        index = index & index_mask;
        index >>= 2;
        return target_vec.at(index);
    }

};

#endif // __JJC__BRANCH__TARGET__BUFFER__H__