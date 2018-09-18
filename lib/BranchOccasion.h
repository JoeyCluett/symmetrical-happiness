#ifndef __JJC__BRANCH__OCCASION__H__
#define __JJC__BRANCH__OCCASION__H__

struct BranchOccasion {
    long int address = 0L;
    bool taken;

    BranchOccasion(long int address, bool taken) {
        this->address = address;
        this->taken = taken;
    }

} __attribute__((packed)); // ... otherwise, waste 7 bytes of space every time

#endif // __JJC__BRANCH__OCCASION__H__