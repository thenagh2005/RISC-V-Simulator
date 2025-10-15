#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "ALU.h"
using namespace std;

int32_t ALU::compute(int op, int32_t operand1, int32_t operand2)
{
    switch (op)
    {
    case 0b000:
        return operand1 + operand2;
    case 0b001:
        return operand1 - operand2;
    case 0b010:
        return operand1 & operand2;
    case 0b011:
        return operand1 | operand2;
    case 0b100:
        return operand1 >> (operand2 & 0x1F); // Arithmetic right shift
    case 0b101:
        return operand1 << (operand2 & 0x1F); // Logical left shift
    default:
        cerr << "Unsupported ALU operation" << endl;
        return 0;
    }
}
