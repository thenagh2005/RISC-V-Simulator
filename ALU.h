#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

enum class ALUOps {
    ADD = 0b000,
    SUB = 0b001,
    AND = 0b010,
    OR = 0b011,
    A_R_SHIFT = 0b100,
    L_SHIFT = 0b101
};

/*
ALUOps get_op_from_bin(int binary) {
    return static_cast<ALUOps>(binary);
}*/

class ALU {
    public:
        int32_t compute(int op, int32_t operand1, int32_t operand2);

};