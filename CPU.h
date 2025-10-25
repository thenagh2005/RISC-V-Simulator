#pragma once

#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "Control.h"
#include "ALU.h"
#include "MUX.h"
using namespace std;

struct Memory {
	vector<uint8_t> mem;
	Memory() { mem.reserve(4096); }

	void ensureSize(size_t size) {
		if (mem.size() < size) {
			mem.resize(size, 0);
		}
	}

	uint8_t readByte(uint32_t addr) {
        ensureSize(addr + 1);
        return mem[addr];
    }
    void writeByte(uint32_t addr, uint8_t val) {
        ensureSize(addr + 1);
        mem[addr] = val;
    }

    // little-endian loads/stores
    uint32_t readWord(uint32_t addr) {
        ensureSize(addr + 4);
        uint32_t w = 0;
        for (int i = 0; i < 4; ++i) {
            w |= ((uint32_t)mem[addr + i]) << (8 * i);
        }
        return w;
    }
    void writeWord(uint32_t addr, uint32_t val) {
        ensureSize(addr + 4);
        for (int i = 0; i < 4; ++i) {
            mem[addr + i] = (uint8_t)((val >> (8 * i)) & 0xFF);
        }
    }

    uint16_t readHalf(uint32_t addr) {
        ensureSize(addr + 2);
        uint16_t h = (uint16_t)mem[addr] | ((uint16_t)mem[addr+1] << 8);
        return h;
    }
    void writeHalf(uint32_t addr, uint16_t val) {
        ensureSize(addr + 2);
        mem[addr] = val & 0xFF;
        mem[addr+1] = (val >> 8) & 0xFF;
    }
};

class instruction
{ // optional
public:
	bitset<32> instr;			   // instruction
	instruction(bitset<32> fetch); // constructor
};

struct InstructionInfo
{
	u_int32_t read_register1 = 0;
	u_int32_t read_register2 = 0;
	u_int32_t write_register = 0;
	int32_t immediate = -1;
	string instruction_name = "NOP";
};

class CPU
{
private:
	int dmemory[1000000];	 // data memory byte addressable in little endian fashion;
	uint32_t PC = 0;		 // pc

	//Register file
	int registers[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // 32 general-purpose registers
	
	//Memory Unit
	Memory mem;

	//Control Unit
	ControlUnit controlUnit;

	//ALU Control
	ALUControl aluControl;

public:
	CPU();

	//Helper function to load bytes from vector input into memory

	void loadBytes(const vector<uint8_t> &bytes) {
        // load bytes into instruction memory starting at address 0
        mem.ensureSize(bytes.size());
        for (size_t i = 0; i < bytes.size(); ++i) mem.mem[i] = bytes[i];
    }
	unsigned long readPC();
	void incPC();
	void setPC(unsigned long newPC);

	void runCycle();

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

	void storeWord(int address, int value)
	{
		if (address % 4 != 0)
		{
			cerr << "Memory out of bounds" << endl;
			//return -1;
		}
		/*
		dmemory[address] = value & 0xFF;
		dmemory[address + 1] = (value >> 8) & 0xFF;
		dmemory[address + 2] = (value >> 16) & 0xFF;
		dmemory[address + 3] = (value >> 24) & 0xFF;*/

		dmemory[address / 4] = value;
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

	void storeByte(int address, int value)
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

	void storeHalfWord(int address, int value)
	{
		if (address % 2 != 0)
		{
			std::cerr << "Misaligned SH at address " << address << std::endl;

			//address = address & ~1; // align address down to even boundary
			exit(1); // RISC-V forbids misaligned halfword access
		}

		int wordIndex = address / 4;
		int halfOffset = (address % 4) / 2;

		uint32_t mask = 0xFFFF << (16 * halfOffset);
		uint32_t newHalf = (value & 0xFFFF) << (16 * halfOffset);

		dmemory[wordIndex] = (dmemory[wordIndex] & ~mask) | newHalf;
	}

	InstructionInfo decode(bitset<32> instr);

	//Methods to interact with register file
	int getRegister(int idx) const;
	void setRegister(int idx, int value);
};

// add other functions and objects here

