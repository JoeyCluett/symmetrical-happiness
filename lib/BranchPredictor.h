#ifndef __JJC__BRANCH__PREDICTOR__H__
#define __JJC__BRANCH__PREDICTOR__H__

#include <iostream>

typedef int64_t addr_t;

enum class Branch : int {
    Taken = 1,
    NotTaken = 0,
    Unknown = -1
};

std::ostream& operator<<(std::ostream& os, Branch br) {
    switch(br) {
        case Branch::Taken:
            os << "BRANCH_TAKEN    "; break;
        case Branch::NotTaken:
            os << "BRANCH_NOT_TAKEN"; break;
        default:
            os << "BRANCH_UNKNOWN  "; break;
    }
    return os;
}

struct BranchOccasion {
    long int address = 0L;
    Branch branch_result;

    BranchOccasion(long int address, Branch taken) {
        this->address = address;
        this->branch_result = taken;
    }

    BranchOccasion(long int address, bool taken) {
        this->address = address;
        this->branch_result = Branch::NotTaken;
        if(taken)
            this->branch_result = Branch::Taken;
    }

} __attribute__((packed)); // ... otherwise, waste 7 bytes of space every time

typedef BranchOccasion BranchOccurance; // IDK WHAT TO NAME THIS THING!! SEND HELP!!

class BranchPredictor {
public:
    virtual bool update(Branch pred, Branch actual) = 0;
    virtual Branch getPrediction(void) = 0;
};

// used for PShare, GShare, Tournament predictors
class SharePredictor {
public:
    virtual bool update(Branch branch) = 0;
    virtual Branch getPrediction(addr_t address) = 0;
    virtual bool simulate(BranchOccasion& bo) = 0; // smells funny...
};

#endif // __JJC__BRANCH__PREDICTOR__H__