#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

struct ControlSignals {
    bool RegWrite;
    bool ALUSrc;
    bool MemRead;
    bool MemWrite;
    bool MemtoReg;
    int ALUOp; // 2-bit control signal
    bool Branch;
    bool Jump;
};