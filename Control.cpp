#include "Control.h"
#include "CPU.h"

#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

// I, U, R, S, B Types

ControlSignals ControlUnit::getSignals(const std::bitset<32> instr)
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
            signals.ALUOp = 0b100; // R-type operation
            break;
        case 0b000: // SUB
            // handle
            signals.ALUOp = 0b001; // R-type operation
            break;
        case 0b111: // AND
            // handle
            signals.ALUOp = 0b010; // R-type operation
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
            signals.ALUOp = 0b000; // I-type operation
            break;
        case 0b110:               // ORI
            signals.ALUOp = 0b011; // I-type operation
            break;
        case 0b011:               // SLTIU
            signals.ALUOp = 0b001; // I-type operation
            break;
        }
    break; 
    case 0b0000011: // LW, LBU
        funct3 = (instr.to_ulong() >> 12) & 0x7; // bits 12-14

        signals.RegWrite = true;
        signals.ALUSrc = true;
        signals.MemRead = true;
        signals.MemWrite = false;
        signals.MemtoReg = true;
        signals.Branch = false;
        signals.ALUOp = 0b000; // I-type operation
    break;
    case 0b0100011: // SW, SH
        // funct3 can be used to differentiate between LW and LBU if needed  
        signals.RegWrite = false;
        signals.ALUSrc = true;
        signals.MemRead = false;
        signals.MemWrite = true; 
        signals.MemtoReg = false;
        signals.Branch = false;
        signals.ALUOp = 0b000; // S-type operation

    break;
    case 0b1100011: // BNE
        funct3 = (instr.to_ulong() >> 12) & 0x7; // bits 12-14

        signals.RegWrite = false;
        signals.ALUSrc = false;
        signals.MemRead = false;
        signals.MemWrite = false;
        signals.MemtoReg = false;
        signals.Branch = true;
        signals.ALUOp = 0b001; // B-type operation
    break;
    case 0b1100111: // JALR
        signals.RegWrite = true;
        signals.ALUSrc = true;
        signals.MemRead = false;
        signals.MemWrite = false;
        signals.MemtoReg = false;
        signals.Branch = false;
        signals.ALUOp = 0b000; // I-type operation
    break;
    }

    return signals;
}   