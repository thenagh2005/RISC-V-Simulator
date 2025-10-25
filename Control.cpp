#include "Control.h"
#include "CPU.h"

#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

// I, U, R, S, B Types

//Function to set control signals based on opcode

ControlSignals ControlUnit::getSignals(uint8_t opcode)
{
    ControlSignals signals;

    uint8_t funct3, funct7;

    switch (opcode)
    {
    case 0x33: // R-type: and, sub, sra

        signals.RegWrite = true;
        signals.MemRead = false;
        signals.MemWrite = false;
        signals.MemtoReg = false;
        signals.ALUSrc = false; // second operand from reg
        signals.Branch = false;
        signals.jalr = false;
        signals.ALUOp = 2; // R-type
        signals.isLui = false;

        break;
    case 0x13: // ADDI, ORI, SLTIU

        signals.RegWrite = true;
        signals.ALUSrc = true;
        signals.MemRead = false;
        signals.MemWrite = false;
        signals.MemtoReg = false;
        signals.Branch = false;
        signals.jalr = false;
        signals.ALUOp = 3;
        signals.isLui = false;

        break;
    case 0x03: // LW, LBU (LOAD instructions)

        signals.RegWrite = true;
        signals.MemRead = true;
        signals.MemWrite = false;
        signals.MemtoReg = true;
        signals.ALUSrc = true; // address = rs1 + imm
        signals.Branch = false;
        signals.jalr = false;
        signals.ALUOp = 0; // ADD for address calc
        signals.isLui = false;

        break;
    case 0x23: // SW, SH
        // funct3 can be used to differentiate between LW and LBU if needed

        signals.RegWrite = false;
        signals.MemRead = false;
        signals.MemWrite = true;
        signals.MemtoReg = false;
        signals.ALUSrc = true; // address = rs1 + imm
        signals.Branch = false;
        signals.jalr = false;
        signals.ALUOp = 0; // ADD
        signals.isLui = false;

        break;
    case 0x63: // BNE

        signals.RegWrite = false;
        signals.MemRead = false;
        signals.MemWrite = false;
        signals.MemtoReg = false;
        signals.ALUSrc = false;
        signals.Branch = true;
        signals.jalr = false;
        signals.ALUOp = 1; // SUB for comparison
        signals.isLui = false;
        break;
    case 0x67: // JALR
        signals.RegWrite = true;
        signals.MemRead = false;
        signals.MemWrite = false;
        signals.MemtoReg = false;
        signals.ALUSrc = true;
        signals.Branch = false;
        signals.jalr = true;
        signals.ALUOp = 0; // ADD for address calc
        signals.isLui = false;
        break;
    case 0x37: // LUI
        signals.RegWrite = true;
        signals.MemRead = false;
        signals.MemWrite = false;
        signals.MemtoReg = false;
        signals.ALUSrc = true;
        signals.Branch = false;
        signals.jalr = false;
        signals.ALUOp = 0;
        signals.isLui = true;
        break;
    default:
        // NOP or unsupported instruction
        break;
    }

    return signals; //Return signals
}