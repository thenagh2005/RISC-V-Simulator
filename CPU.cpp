#include "CPU.h"


CPU::CPU()
{
	PC = 0; //set PC to 0
	for (int i = 0; i < 4096; i++) //copy instrMEM
	{
		dmemory[i] = (0);
	}
}


unsigned long CPU::readPC()
{
	return PC;
}
void CPU::incPC()
{
	PC++;
}

// Implements the RISC-V ADDI instruction: rd = rs + imm
void CPU::ADDI(int rd, int rs, int imm)
{
	if (rd < 0 || rd >= 32 || rs < 0 || rs >= 32) {
		cerr << "Register index out of bounds in ADDI" << endl;
		return;
	}
	registers[rd] = registers[rs] + imm;
}

//Implements the RISC-V AND instruction: rd = rs1 & rs2
void CPU::AND(int rd, int rs1, int rs2) {
	if (rd < 0 || rd >= 32 || rs1 < 0 || rs1 >= 32 || rs2 < 0 || rs2 >= 32) {
		cerr << "Register index out of bounds in AND" << endl;
		return;
	}
	registers[rd] = registers[rs1] & registers[rs2];
}

//Implements the RISC-V ORI instruction: rd = rs | imm
void CPU::ORI(int rd, int rs, int imm)
{
	if (rd < 0 || rd >= 32 || rs < 0 || rs >= 32) {
		cerr << "Register index out of bounds in ORI" << endl;
		return;
	}
	registers[rd] = registers[rs] | imm;
}

//Implements the SUB instruction: rd = rs1 - rs2
void CPU::SUB(int rd, int rs1, int rs2)
{
	if (rd < 0 || rd >= 32 || rs1 < 0 || rs1 >= 32 || rs2 < 0 || rs2 >= 32) {
		cerr << "Register index out of bounds in SUB" << endl;
		return;
	}
	registers[rd] = registers[rs1] - registers[rs2];
}

//Implements the SRA instruction: rd = rs1 >> rs2 (arithmetic right shift)
void CPU::SRA(int rd, int rs1, int rs2)
{
	if (rd < 0 || rd >= 32 || rs1 < 0 || rs1 >= 32 || rs2 < 0 || rs2 >= 32) {
		cerr << "Register index out of bounds in SRA" << endl;
		return;
	}
	int shiftAmount = registers[rs2]; // RISC-V uses only the lower 5 bits for shift amount
	registers[rd] = registers[rs1] >> shiftAmount; // Arithmetic right shift
}

void CPU::SLTIU(int rd, int rs, int imm) {
	if (rd < 0 || rd >= 32 || rs < 0 || rs >= 32) {
		cerr << "Register index out of bounds in SLTIU" << endl;
		return;
	}
	uint rs_val = static_cast<uint>(registers[rs]);
	uint imm_val = static_cast<uint>(imm);

	registers[rd] = (rs_val < imm_val) ? 1 : 0;
}

void CPU::LW(int rd, int rs1, int offset) {
	if (rd < 0 || rd >= 32 || rs1 < 0 || rs1 >= 32) {
		cerr << "Register index out of bounds in LW" << endl;
		return;
	}
	int address = registers[rs1] + offset;
	if (address < 0 || address + 3 >= 4096) {
		cerr << "Memory access out of bounds in LW" << endl;
		return;
	}
	// Load 4 bytes from memory and combine them into a single integer
	registers[rd] = (dmemory[address] & 0xFF) |
					((dmemory[address + 1] & 0xFF) << 8) |
					((dmemory[address + 2] & 0xFF) << 16) |
					((dmemory[address + 3] & 0xFF) << 24);
}

void CPU::SW(int rs2, int rs1, int offset) {
	if (rs2 < 0 || rs2 >= 32 || rs1 < 0 || rs1 >= 32) {
		cerr << "Register index out of bounds in SW" << endl;
		return;
	}
	int address = registers[rs1] + offset;
	if (address < 0 || address + 3 >= 4096) {
		cerr << "Memory access out of bounds in SW" << endl;
		return;
	}
	// Store the integer into 4 bytes in memory
	dmemory[address] = registers[rs2] & 0xFF;
	dmemory[address + 1] = (registers[rs2] >> 8) & 0xFF;
	dmemory[address + 2] = (registers[rs2] >> 16) & 0xFF;
	dmemory[address + 3] = (registers[rs2] >> 24) & 0xFF;
}

void CPU::LBU(int rd, int rs1, int offset) {
	if (rd < 0 || rd >= 32 || rs1 < 0 || rs1 >= 32) {
		cerr << "Register index out of bounds in LBU" << endl;
		return;
	}
	int address = registers[rs1] + offset;
	if (address < 0 || address >= 4096) {
		cerr << "Memory access out of bounds in LBU" << endl;
		return;
	}
	// Load a single byte from memory and zero-extend it to an integer
	registers[rd] = dmemory[address] & 0xFF;
}

void CPU::BNE(int rs1, int rs2, int imm) {
	if (rs1 < 0 || rs1 >= 32 || rs2 < 0 || rs2 >= 32) {
		cerr << "Register index out of bounds in BNE" << endl;
		return;
	}
	if (registers[rs1] != registers[rs2]) {
		PC += imm; // Branch by adding immediate to PC
	} else {
		PC++; // Increment PC normally if not branching
	}
}

void CPU::JALR(int rd, int rs1, int offset) {
	registers[rd] = PC + 1;
	PC = (registers[rs1] + offset) & ~1;
}

void CPU::LUI(int rd, int imm) {
	if (rd < 0 || rd >= 32) {
		cerr << "Register index out of bounds in LUI" << endl;
		return;
	}
	registers[rd] = imm << 12; // Load upper immediate
}

int CPU::getRegister(int idx) const {
	if (idx < 0 || idx >= 32) {
		cerr << "Register index out of bounds in getRegister" << endl;
		return 0;
	}
	return registers[idx];
}

void CPU::setRegister(int idx, int value) {
	if (idx < 0 || idx >= 32) {
		cerr << "Register index out of bounds in setRegister" << endl;
		return;
	}
	registers[idx] = value;
}
// Add other functions here ... 
