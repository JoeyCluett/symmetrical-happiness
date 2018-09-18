#ifndef __JJC__SHARE__H__
#define __JJC__SHARE__H__

#include "BranchPredictor.h"

class Share {
public:

    virtual Branch getPrediction(addr_t address) = 0;
    virtual bool update(Branch branch) = 0;

};

#endif // __JJC__SHARE__H__