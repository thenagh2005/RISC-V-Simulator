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

struct InstructionInfo
{
	u_int32_t read_register1 = -1;
	u_int32_t read_register2 = -1;
	u_int32_t write_register = -1;
	int32_t immediate = -1;
	string instruction_name = "NOP";
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
	void setPC(unsigned long newPC);

	int loadWord(int address) {

	}

	int storeWord(int address, int value) {

	}

	int loadByte(int address) {

	}

	int storeByte(int address, int value) {

	}

	int loadHalfWord(int address) {

	}

	int storeHalfWord(int address, int value) {

	}

	InstructionInfo decode(bitset<32> instr);

	void ADDI(int rd, int rs, int imm);
	void LUI(int rd, int imm);
	void ORI(int rd, int rs, int imm);
	void SLTIU(int rd, int rs, int imm);
	void SRA(int rd, int rs1, int rs2);
	void SUB(int rd, int rs1, int rs2);
	void AND(int rd, int rs1, int rs2);
	void LW(int rd, int rs1, int offset);
	void SW(int rs2, int rs1, int offset);
	void LBU(int rd, int rs1, int offset);
	void BNE(int rs1, int rs2, int imm);
	void JALR(int rd, int rs1, int offset);
	// Optionally, add methods to get/set registers for testing
	int getRegister(int idx) const;
	void setRegister(int idx, int value);
};

// add other functions and objects here
