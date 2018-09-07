#ifndef __JJC__TWO__BIT__COUNTER__H__
#define __JJC__TWO__BIT__COUNTER__H__

#include <iostream>

enum class TBC : int {
    StronglyNotTaken = 0,
    WeaklyNotTaken   = 1,
    WeaklyTaken      = 2,
    StronglyTaken    = 3,
    Unknown          = -1
};

enum class Branch : int {
    Taken = 1,
    NotTaken = 0,
    Unknown = -1
};

std::ostream& operator<<(std::ostream& os, TBC tbc) {
    switch(tbc) {
        case TBC::StronglyNotTaken:
            os << "STRONGLY_NOT_TAKEN"; break;
        case TBC::WeaklyNotTaken:
            os << "WEAKLY_NOT_TAKEN  "; break;
        case TBC::WeaklyTaken:
            os << "WEAKLY_TAKEN      "; break;
        case TBC::StronglyTaken:
            os << "STRONGLY_TAKEN    "; break;
        default:
            os << "UNKNOWN_TBC_STATE "; break;
    }
    return os;
}

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

class TwoBitCounter {
private:
    TBC state;

public:
    TwoBitCounter(TBC state = TBC::StronglyTaken) {
        this->state = state;
    }

    TBC getState(void) {
        return this->state;
    }

    Branch getPrediction() {
        switch(this->state) {
            case TBC::StronglyNotTaken:
            case TBC::WeaklyNotTaken:
                return Branch::NotTaken;

            case TBC::StronglyTaken:
            case TBC::WeaklyTaken:
                return Branch::Taken;
            
            default:
                return Branch::Unknown;
        }
    }

    TBC update(Branch NT) {
        switch(state) {
            case TBC::StronglyNotTaken:
                if(NT == Branch::Taken)
                    this->state = TBC::WeaklyNotTaken;
                break;
            case TBC::WeaklyNotTaken:
                if(NT == Branch::Taken)
                    this->state = TBC::WeaklyTaken;
                else
                    this->state = TBC::StronglyNotTaken;
                break;
            case TBC::WeaklyTaken:
                if(NT == Branch::Taken)
                    this->state = TBC::StronglyTaken;
                else
                    this->state = TBC::WeaklyNotTaken;
                break;
            case TBC::StronglyTaken:
                if(NT == Branch::NotTaken)
                    this->state = TBC::WeaklyTaken;
                break;
            default:
                return TBC::Unknown;
        }

        return this->state;
    }

    TBC operator()(Branch NT) {
        return this->update(NT);
    }

    auto acr(void) -> const char* {
        switch(this->state) {
            case TBC::StronglyNotTaken:
                return "SN";
            case TBC::StronglyTaken:
                return "ST";
            case TBC::WeaklyNotTaken:
                return "WN";
            case TBC::WeaklyTaken:
                return "WT";
            default:
                return "ERROR";
        }
    }

};

std::ostream& operator<<(std::ostream& os, TwoBitCounter& tbc) {
    os << tbc.getState();
    return os;
}

#endif // __JJC__TWO__BIT__COUNTER__H__
