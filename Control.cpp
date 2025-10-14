#include "Control.h"
#include "CPU.h"

#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;    

ControlSignals ControlUnit::decode(const std::bitset<32> instr) {
    uint8_t opcode = instr.to_ulong() & 0x7F; // Extract opcode (bits 0-6)
    ControlSignals signals = {};

    switch (opcode) {
        case 0b0110011: // R-type
            signals.RegWrite = true;
            signals.ALUSrc = false;
            signals.MemRead = false;
            signals.MemWrite = false;
            signals.MemtoReg = false;
            signals.ALUOp = 0b10; // ALU operation determined by funct3 and funct7
            signals.Branch = false;
            signals.Jump = false;
            break;
        case 0b0010011: // I-type (e.g., ADDI, ORI)
            signals.RegWrite = true;
            signals.ALUSrc = true;
            signals.MemRead = false;
            signals.MemWrite = false;
            signals.MemtoReg = false;
            signals.ALUOp = 0b00; // ALU operation determined by funct3
            signals.Branch = false;
            signals.Jump = false;
            break;
        case 0b0000011: // Load (e.g., LW, LBU, LUI)
            signals.RegWrite = true;
            signals.ALUSrc = true;
            signals.MemRead = true;
            signals.MemWrite = false;
            signals.MemtoReg = true;
            signals.ALUOp = 0b00; // ALU operation is addition for address calculation
            signals.Branch = false;
            signals.Jump = false;
            break;
        case 0b0100011: // Store (e.g., SW)
            signals.RegWrite = false;
            signals.ALUSrc = true;
            signals.MemRead = false;
            signals.MemWrite = true;
            signals.MemtoReg = false; // Don't care
            signals.ALUOp = 0b00; // ALU operation is addition for address calculation
            signals.Branch = false;
            signals.Jump = false;
            break;
        case 0b1100011: // Branch (e.g., BNE)
            signals.RegWrite = false;
            signals.ALUSrc = false;
            signals.MemRead = false;
            signals.MemWrite = false;
            signals.MemtoReg = false; // Don't care
            signals.ALUOp = 0b01; // ALU operation is subtraction for comparison
            signals.Branch = true;
            signals.Jump = false;
            break;
        case 0b1100111: // JALR
            signals.RegWrite = true;
            signals.ALUSrc = true;
            signals.MemRead = false;
            signals.MemWrite = false;
            signals.MemtoReg = false; // PC + 4 to rd
            signals.ALUOp = 0b00; // ALU operation is addition for address calculation
            signals.Branch = false;
            signals.Jump = true;
            break;
    } 
    
    return signals;
}