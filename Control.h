#pragma once
#include <stdint.h>

#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

struct ControlSignals {
    bool RegWrite = false;
    bool ALUSrc = false;
    bool MemRead = false;
    bool MemWrite = false;
    bool MemtoReg = false;
    int ALUOp = 0b00; // 2-bit control signal
    bool Branch = false;
    bool jalr = false; // 2 - bits: 00 - byte, 01 - halfword, 10 - word, 11 - irrelevant
    bool isLui = false;
};

class ControlUnit {
    public:
        ControlSignals getSignals(uint8_t opcode); //Get signals from opcode
};