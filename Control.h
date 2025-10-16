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
    int ALUOp; // 3-bit control signal
    bool Branch;
    int size; // 2 - bits: 00 - byte, 01 - halfword, 10 - word, 11 - irrelevant
    bool Jump;
};

class ControlUnit {
    public:
        ControlSignals getSignals(const std::bitset<32> instr);
};