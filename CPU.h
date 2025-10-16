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
	int dmemory[1000000];	 // data memory byte addressable in little endian fashion;
	unsigned long PC;		 // pc
	int registers[32] = {0}; // 32 general-purpose registers

public:
	CPU();
	unsigned long readPC();
	void incPC();
	void setPC(unsigned long newPC);

	int loadWord(int address)
	{
		if (address % 4 != 0)
		{
			cerr << "Memory out of bounds" << endl;
			return 0;
		}
		/*
		int word = (dmemory[address] & 0xFF) |
			   ((dmemory[address + 1] & 0xFF) << 8) |
			   ((dmemory[address + 2] & 0xFF) << 16) |
			   ((dmemory[address + 3] & 0xFF) << 24);*/

		int word = dmemory[address / 4];

		return word;
	}

	int storeWord(int address, int value)
	{
		if (address % 4 != 0)
		{
			cerr << "Memory out of bounds" << endl;
			return -1;
		}
		/*
		dmemory[address] = value & 0xFF;
		dmemory[address + 1] = (value >> 8) & 0xFF;
		dmemory[address + 2] = (value >> 16) & 0xFF;
		dmemory[address + 3] = (value >> 24) & 0xFF;*/

		dmemory[address / 4] = value;
		return 0;
	}

	int loadByte(int address)
	{
		int wordIndex = address / 4;  // which word
		int byteOffset = address % 4; // which byte (0–3)

		uint32_t word = dmemory[wordIndex];
		int8_t byte = (word >> (8 * byteOffset)) & 0xFF; // extract byte

		return (int32_t)byte; // sign-extend to 32 bits
	}

	uint32_t loadByteU(int address) {
		int wordIndex = address / 4;
		int byteOffset = address % 4;

		uint32_t word = dmemory[wordIndex];
		uint32_t byte = (word >> (8 * byteOffset)) & 0xFF;
		return (uint32_t)byte; // zero-extend to 32 bits
	}

	int storeByte(int address, int value)
	{
		int wordIndex = address / 4;  // which word
		int byteOffset = address % 4; // which byte inside word

		uint32_t mask = 0xFF << (8 * byteOffset); // mask for target byte
		uint32_t newByte = (value & 0xFF) << (8 * byteOffset);

		dmemory[wordIndex] = (dmemory[wordIndex] & ~mask) | newByte;
	}

	int loadHalfWord(int address)
	{
		if (address % 2 != 0)
		{
			std::cerr << "Misaligned LH at address " << address << std::endl;
			exit(1); // RISC-V forbids misaligned halfword access
		}

		int wordIndex = address / 4;
		int halfOffset = (address % 4) / 2;

		uint32_t word = dmemory[wordIndex];
		int16_t half = (word >> (16 * halfOffset)) & 0xFFFF;

		return (int32_t)half; // sign-extend to 32 bits
	}

	int storeHalfWord(int address, int value)
	{
		if (address % 2 != 0)
		{
			std::cerr << "Misaligned SH at address " << address << std::endl;
			exit(1); // RISC-V forbids misaligned halfword access
		}

		int wordIndex = address / 4;
		int halfOffset = (address % 4) / 2;

		uint32_t mask = 0xFFFF << (16 * halfOffset);
		uint32_t newHalf = (value & 0xFFFF) << (16 * halfOffset);

		dmemory[wordIndex] = (dmemory[wordIndex] & ~mask) | newHalf;
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
