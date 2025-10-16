
#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

/*
Add all the required standard and developed libraries here
*/
#include "Control.h"
#include "CPU.h"
#include "ALU.h"
#include "MUX.h"

/*
Put/Define any helper function/definitions you need here
*/

unsigned char hexCharToByte(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return 10 + (c - 'a');
	if (c >= 'A' && c <= 'F')
		return 10 + (c - 'A');
	return 0;
}

int main(int argc, char *argv[])
{
	/* This is the front end of your project.
	You need to first read the instructions that are stored in a file and load them into an instruction memory.
	*/

	/* Each cell should store 1 byte. You can define the memory either dynamically, or define it as a fixed size with size 4KB (i.e., 4096 lines). Each instruction is 32 bits (i.e., 4 lines, saved in little-endian mode).
	Each line in the input file is stored as an hex and is 1 byte (each four lines are one instruction). You need to read the file line by line and store it into the memory. You may need a mechanism to convert these values to bits so that you can read opcodes, operands, etc.
	*/

	char instMem[4096];

	if (argc < 2)
	{
		// cout << "No file name entered. Exiting...";
		return -1;
	}

	ifstream infile(argv[1]); // open the file
	if (!(infile.is_open() && infile.good()))
	{
		cout << "error opening file\n";
		return 0;
	}

	string line;
	int i = 0;
	while (infile)
	{
		infile >> line;
		stringstream line2(line);
		char x;
		line2 >> x;
		instMem[i] = x; // be careful about hex
		i++;
		line2 >> x;
		instMem[i] = x; // be careful about hex
		//cout << instMem[i] << endl;
		i++;
	}
	int maxPC = i / 4;
	/*
	string line;
	int i = 0;
	while (infile)
	{
		infile >> line;
		if (line.empty())
			continue; // skip empty lines

		unsigned int val;
		stringstream line2(line);
		line2 >> hex >> val; // convert ASCII hex to numeric byte

		instMem[i] = static_cast<unsigned char>(val & 0xFF);
		//cout << "Loaded byte " << i << ": 0x" << hex << val << dec << endl;
		i++;
	}*/
	//int maxPC = i / 4;

	for (int pc = 0; pc < maxPC; ++pc) {
        int idx = pc * 8;
        unsigned char bytes[4];

        // convert each pair of ASCII chars to a numeric byte
        for (int i = 0; i < 4; ++i) {
            bytes[i] = (hexCharToByte(instMem[idx + 2*i]) << 4) | hexCharToByte(instMem[idx + 2*i + 1]);
        }

        // assemble into 32-bit instruction (little-endian)
        uint32_t instr32 =
            (uint32_t)bytes[0] |
            ((uint32_t)bytes[1] << 8) |
            ((uint32_t)bytes[2] << 16) |
            ((uint32_t)bytes[3] << 24);

        bitset<32> instructionBits(instr32);

        //cout << "Instruction " << pc << ": 0x" << hex << instr32 << "  " << instructionBits << endl;
		
    }

	for (int j = 0; j < i; j++)
	{
		//cout << "Hello " << (unsigned char)instMem[j] << endl;
	}

	/* Instantiate your CPU object here.  CPU class is the main class in this project that defines different components of the processor.
	CPU class also has different functions for each stage (e.g., fetching an instruction, decoding, etc.).
	*/

	CPU myCPU; // call the approriate constructor here to initialize the processor...
	// make sure to create a variable for PC and resets it to zero (e.g., unsigned int PC = 0);
	unsigned int PC = 0;
	ControlUnit controlUnit;
	ALU alu1; // ALU that is between registers and data memory
	ALU alu2; // ALU that is used for branch calculations
	ALU alu3; // ALU that is used to calculate the new PC value when a jump instruction is encountered

	MUX mux1; // MUX that selects between immediate and register value for ALU input
	MUX mux2; // MUX that selects between ALU result and data memory output for writing
	MUX mux3; // MUX that selects between PC+4 and branch target for next PC value

	int funct7, funct3, rs1, rs2, rd, opcode;
	int regWrite, ALUSrc, MemRead, MemWrite, MemtoReg, Branch, size;

	int ALUOp;

	/* OPTIONAL: Instantiate your Instruction object here. */
	// instruction myInst;

	bool done = true;
	while (done == true) // processor's main loop. Each iteration is equal to one clock cycle.
	{
		// fetch

		unsigned long PC = myCPU.readPC();

		if (PC * 4 + 3 >= 4096)
		{
			cerr << "PC out of bounds" << endl;
			break;
		}

		int idx = PC * 8;
        unsigned char bytes[4];

        // convert each pair of ASCII chars to a numeric byte
        for (int i = 0; i < 4; ++i) {
            bytes[i] = (hexCharToByte(instMem[idx + 2*i]) << 4) | hexCharToByte(instMem[idx + 2*i + 1]);
        }

        // assemble into 32-bit instruction (little-endian)
        uint32_t instr32 =
            (uint32_t)bytes[0] |
            ((uint32_t)bytes[1] << 8) |
            ((uint32_t)bytes[2] << 16) |
            ((uint32_t)bytes[3] << 24);

        bitset<32> instruction(instr32);

        //cout << "Instruction " << PC << ": 0x" << hex << instr32 << "  " << instruction << endl;

		ControlSignals signals = controlUnit.getSignals(instruction); // Got the control signals
		InstructionInfo info = myCPU.decode(instruction);			  // Decoded the instruction

		string instrName = info.instruction_name;
		uint32_t rdName = info.write_register;
		uint32_t rs1Name = info.read_register1;
		uint32_t rs2Name = info.read_register2;
		int32_t imm = info.immediate;

		regWrite = signals.RegWrite; //Load the control signals into variables
		ALUSrc = signals.ALUSrc;
		MemRead = signals.MemRead;
		MemWrite = signals.MemWrite;
		MemtoReg = signals.MemtoReg;
		ALUOp = signals.ALUOp;
		Branch = signals.Branch;
		size = signals.size;

		/*

		int32_t reg1 = myCPU.getRegister(rs1Name);
		int32_t reg2 = myCPU.getRegister(rs2Name);

		int32_t aluB = mux1.select(ALUSrc, reg2, imm);

		int32_t aluResult = alu1.compute(ALUOp, reg1, aluB);

		int32_t memData = 0;

		if (MemRead) {
			if (size == 0b00) {
				memData = myCPU.loadByte(aluResult);
			} else if (size == 0b10) {
				memData = myCPU.loadWord(aluResult);
			}
		}

		if (MemWrite) {
			if (size == 0b01) {
				myCPU.storeHalfWord(aluResult, reg2);
			} else if (size == 0b10) {
				myCPU.storeWord(aluResult, reg2);
			}
		}

		if (regWrite) {
			int32_t writeData = mux2.select(MemtoReg, aluResult, memData);
			myCPU.setRegister(rdName, writeData);
		}*/

		
		//cout << instrName << ", " << rdName << ", " << rs1Name << ", " << rs2Name << ", " << imm << endl;

		//Begin cycle
		int read_data1 = 0;
		int read_data2 = 0;

		int from_memory = 0;

		if (rs1Name != -1) {
			read_data1 = myCPU.getRegister(rs1Name);
		}
		if (rs2Name != -1) {
			read_data2 = myCPU.getRegister(rs2Name);
		}

		int32_t alu_input1 = read_data1;
		int32_t alu_input2 = mux1.select(ALUSrc, read_data2, imm);
		int32_t alu_result;

		if (instrName == "LUI") {
			int alu_input1 = 12; //If LUI, shift immediate by 12 bits
			alu_result = alu1.compute(ALUOp, alu_input2, alu_input1);
		} else {
			alu_result = alu1.compute(ALUOp, alu_input1, alu_input2);
		}

		//Now memory
		
		if (MemRead) {
			if (size == 0b00) { // LBU
				from_memory = myCPU.loadByte(alu_result);
				cout << "Loaded from memory: " << alu_result << endl;
			} else if (size == 0b10) { // LW
				from_memory = myCPU.loadWord(alu_result);
				cout << "Loaded from memory: " << alu_result << endl;
			}
		}

		int32_t write_data = mux2.select(MemtoReg, alu_result, from_memory);

		int32_t mem_write_data = read_data2;

		
		if (regWrite && rdName != -1) {
			myCPU.setRegister(rdName, write_data);
		}

		if (MemWrite) {
			if (size == 0b10) { // SW
				myCPU.storeWord(alu_result, mem_write_data);
				cout << "Stored to memory: " << alu_result << " value: " << mem_write_data << endl;
			} else if (size == 0b01) { // SH
				myCPU.storeHalfWord(alu_result, mem_write_data);
				cout << "Stored to memory: " << alu_result << " value: " << mem_write_data << endl;
			} 
		}

		
		// ...
		myCPU.incPC();
		if (myCPU.readPC() > maxPC)
			break;
	}

	cout << myCPU.getRegister(10) << " " << myCPU.getRegister(11) << endl;
	int a0 = 0;
	int a1 = 0;
	// print the results (you should replace a0 and a1 with your own variables that point to a0 and a1)
	cout << "(" << myCPU.getRegister(10) << "," << myCPU.getRegister(11) << ")" << endl;

	return 0;
}