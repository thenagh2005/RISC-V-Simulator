#pragma once
#include <cstdint>
#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

//Possible ALU operations
enum ALUOps {
    ALU_ADD = 0,
    ALU_SUB,
    ALU_AND,
    ALU_OR,
    ALU_SRA,
    ALU_SLTU,
    ALU_PASS, // passthrough (used by LUI handling separately)
};


struct ALUControl {
    // ALU Control receives ALUOp (from main controller) + funct3 + funct7
    // and returns an ALUOps value.
    // We use ALUOp codes:
    //   0 -> ADD (default for loads/stores/addi)
    //   1 -> SUB  (for branches comparison)
    //   2 -> R-TYPE (use funct3/funct7 to determine)
    //   3 -> I-TYPE (OP-IMM: use funct3)
    

    ALUControl() {} //Empty constructor

    //Function to decide ALU operation based on ALUOp, funct3, and funct7

    ALUOps getALUControl(int ALUOp, uint8_t funct3, uint8_t funct7_low7) {
        if (ALUOp == 0) return ALU_ADD;
        if (ALUOp == 1) return ALU_SUB;
        if (ALUOp == 3) {
            // I-type (OP-IMM): use funct3
            switch (funct3) {
                case 0x0: return ALU_ADD;   // ADDI
                case 0x6: return ALU_OR;    // ORI
                case 0x3: return ALU_SLTU;  // SLTIU (unsigned compare)
                
                default: return ALU_ADD;
            }
        }
        if (ALUOp == 2) {
            // R-type: decide from funct3/funct7
            switch (funct3) {
                case 0x0:
                    if (funct7_low7 == 0x20) return ALU_SUB; // SUB
                case 0x5:
                    if (funct7_low7 == 0x20) return ALU_SRA; // SRA
                case 0x7:
                    return ALU_AND; // AND
                case 0x6:
                    return ALU_OR; // OR
                default:
                    return ALU_ADD;
            }
        }
        return ALU_ADD;
    }
};


//ALU returns the integer result as well as boolean flag for whether it is 0

struct ALUResult {
    int32_t result;
    bool zero;
};

struct ALU {

    //Function to do the operation
    static ALUResult operate(ALUOps op, int32_t a, int32_t b);
};