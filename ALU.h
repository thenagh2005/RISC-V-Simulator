#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

enum class ALUOps {
    ADD = 0b00,
    SUB = 0b01,
    AND = 0b10,
    OR = 0b11,
};

class ALU {
    public:
        int32_t compute(ALUOps op, int32_t operand1, int32_t operand2);

};