#ifndef __JJC__BRANCH__PREDICTOR__H__
#define __JJC__BRANCH__PREDICTOR__H__

#include <iostream>

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

class BranchPredictor {
public:
    virtual bool update(Branch pred, Branch actual) = 0;
    virtual Branch getPrediction(void) = 0;
};

#endif // __JJC__BRANCH__PREDICTOR__H__