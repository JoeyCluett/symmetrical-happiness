#ifndef __JJC__EXECUTION__UNIT__H__
#define __JJC__EXECUTION__UNIT__H__

#include "RegisterFile.h"

// could just use structs at this point
class ExecutionUnit {
public:
    int busy = false;
    
    int operation;
    int cycles_to_complete;

    // increment every clock cycle
    int current_cycles = 0;

    MultiType output;
    MultiType input_1;
    MultiType input_2;
};

#endif // __JJC__EXECUTION__UNIT__H__