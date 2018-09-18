#ifndef __JJC__MIPS__CONSTANTS__H__
#define __JJC__MIPS__CONSTANTS__H__

#include <map>
#include <string>

namespace MIPS_inst {

    const int add  = 0;
    const int lw   = 1;
    const int sw   = 2;
    const int addi = 3;
    const int bne  = 4;
    const int slt  = 5;
    const int b    = 6;
    const int beq  = 7;
    const int halt = 8;

};

namespace MIPS_reg {
    const int zero  = 0;
    const int at    = 1;
    const int v[2]  = {2, 3};
    const int a[4]  = {4, 5, 6, 7, };
    const int t[10] = {8, 9, 10, 11, 12, 13, 14, 15, 24, 25};
    const int s[8]  = {16, 17, 18, 19, 20, 21, 22, 23};
    const int k[2]  = {26, 27};
    const int gp    = 28;
    const int sp    = 29;
    const int fp    = 30;
    const int ra    = 31;
};

#endif // __JJC__MIPS__CONSTANTS__H__