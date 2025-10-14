#include "Control.h"
#include "CPU.h"

#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

// I, U, R, S, B Types

ControlSignals ControlUnit::decode(const std::bitset<32> instr)
{
    uint8_t opcode = instr.to_ulong() & 0x7F; // Extract opcode (bits 0-6)
    ControlSignals signals = {};

    uint8_t funct3, funct7;

    switch (opcode)
    {
    case 0b0110011:                                      // SRA, SUB, AND
        funct3 = (instr.to_ulong() >> 12) & 0x7; // bits 12-14

        signals.RegWrite = true;
        signals.ALUSrc = false;
        signals.MemRead = false;
        signals.MemWrite = false;
        signals.MemtoReg = false;
        signals.Branch = false;

        switch (funct3)
        {
        case 0b101: // SRA
            // handle
            signals.ALUOp = 0b11; // R-type operation
            break;
        case 0b000: // SUB
            // handle
            signals.ALUOp = 0b01; // R-type operation
            break;
        case 0b111: // AND
            // handle
            signals.ALUOp = 0b10; // R-type operation
            break;
        }
    break;
    case 0b0010011:                                        // ADDI, ORI, SLTIU
        funct3 = (instr.to_ulong() >> 12) & 0x7; // bits 12-14

        signals.RegWrite = true;
        signals.ALUSrc = true;
        signals.MemRead = false;
        signals.MemWrite = false;
        signals.MemtoReg = false;
        signals.Branch = false;

        switch (funct3)
        {
        case 0b000:               // ADDI
            signals.ALUOp = 0b00; // I-type operation
            break;
        case 0b110:               // ORI
            signals.ALUOp = 0b11; // I-type operation
            break;
        case 0b011:               // SLTIU
            signals.ALUOp = 0b01; // I-type operation
            break;
        }
    break;    
    }

    return signals;
}   