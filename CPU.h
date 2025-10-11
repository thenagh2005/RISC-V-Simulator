#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

class instruction
{ // optional
public:
	bitset<32> instr;			   // instruction
	instruction(bitset<32> fetch); // constructor
};

class CPU
{
private:
	int dmemory[4096]; // data memory byte addressable in little endian fashion;
	unsigned long PC;  // pc
	int registers[32] = {0}; // 32 general-purpose registers

public:
	CPU();
	unsigned long readPC();
	void incPC();
	void ADDI(int rd, int rs, int imm);
	void LUI();
	void ORI(int rd, int rs, int imm);
	void SLTIU();
	void SRA();
	void SUB(int rd, int rs1, int rs2);
	void AND(int rd, int rs1, int rs2);
	void LBU();
	// Optionally, add methods to get/set registers for testing
	int getRegister(int idx) const;
	void setRegister(int idx, int value);
};

// add other functions and objects here
