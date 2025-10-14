#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "ALU.h"
using namespace std;

int32_t ALU::compute(ALUOps op, int32_t operand1, int32_t operand2)
{
    switch (op)
    {
    case ALUOps::ADD:
        return operand1 + operand2;
    case ALUOps::SUB:
        return operand1 - operand2;
    case ALUOps::AND:
        return operand1 & operand2;
    case ALUOps::OR:
        return operand1 | operand2;
    case ALUOps::A_R_SHIFT:
        return operand1 >> (operand2 & 0x1F); // Arithmetic right shift
    case ALUOps::L_SHIFT:
        return operand1 << (operand2 & 0x1F); // Logical left shift
    default:
        cerr << "Unsupported ALU operation" << endl;
        return 0;
    }
}
