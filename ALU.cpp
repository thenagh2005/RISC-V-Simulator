#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "ALU.h"
using namespace std;

//Function to get ALU result based on operation, and inputs

ALUResult ALU::operate(ALUOps op, int32_t a, int32_t b) {
        ALUResult r;
        switch (op) {
            case ALU_ADD:
                r.result = (int32_t)((int64_t)a + (int64_t)b);
                break;
            case ALU_SUB:
                r.result = (int32_t)((int64_t)a - (int64_t)b);
                break;
            case ALU_AND:
                r.result = a & b;
                break;
            case ALU_OR:
                r.result = a | b;
                break;
            case ALU_SRA: {
                // arithmetic right shift by lower 5 bits of b
                int sh = (int)(b & 0x1F);
                // use implementation-defined right shift on signed types,
                // which on two's complement machines is arithmetic in practice.
                r.result = a >> sh;
                break;
            }
            case ALU_SLTU: {
                // Set if a < b (unsigned)
                uint32_t ua = (uint32_t)a, ub = (uint32_t)b;
                r.result = (ua < ub) ? 1 : 0;
                break;
            }
            case ALU_PASS:
                r.result = b;
                break;
            default:
                r.result = 0;
        }
        r.zero = (r.result == 0);
        return r;
    }

