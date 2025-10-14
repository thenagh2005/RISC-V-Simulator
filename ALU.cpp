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
    default:
        cerr << "Unsupported ALU operation" << endl;
        return 0;
    }
}
