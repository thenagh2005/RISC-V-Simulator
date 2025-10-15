#include "CPU.h"

CPU::CPU()
{
	PC = 0;						   // set PC to 0
	for (int i = 0; i < 4096; i++) // copy instrMEM
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

void CPU::setPC(unsigned long newPC)
{
	PC = newPC;
}

// Implements the RISC-V ADDI instruction: rd = rs + imm
void CPU::ADDI(int rd, int rs, int imm)
{
	if (rd < 0 || rd >= 32 || rs < 0 || rs >= 32)
	{
		cerr << "Register index out of bounds in ADDI" << endl;
		return;
	}
	registers[rd] = registers[rs] + imm;
}

// Implements the RISC-V AND instruction: rd = rs1 & rs2
void CPU::AND(int rd, int rs1, int rs2)
{
	if (rd < 0 || rd >= 32 || rs1 < 0 || rs1 >= 32 || rs2 < 0 || rs2 >= 32)
	{
		cerr << "Register index out of bounds in AND" << endl;
		return;
	}
	registers[rd] = registers[rs1] & registers[rs2];
}

// Implements the RISC-V ORI instruction: rd = rs | imm
void CPU::ORI(int rd, int rs, int imm)
{
	if (rd < 0 || rd >= 32 || rs < 0 || rs >= 32)
	{
		cerr << "Register index out of bounds in ORI" << endl;
		return;
	}
	registers[rd] = registers[rs] | imm;
}

// Implements the SUB instruction: rd = rs1 - rs2
void CPU::SUB(int rd, int rs1, int rs2)
{
	if (rd < 0 || rd >= 32 || rs1 < 0 || rs1 >= 32 || rs2 < 0 || rs2 >= 32)
	{
		cerr << "Register index out of bounds in SUB" << endl;
		return;
	}
	registers[rd] = registers[rs1] - registers[rs2];
}

// Implements the SRA instruction: rd = rs1 >> rs2 (arithmetic right shift)
void CPU::SRA(int rd, int rs1, int rs2)
{
	if (rd < 0 || rd >= 32 || rs1 < 0 || rs1 >= 32 || rs2 < 0 || rs2 >= 32)
	{
		cerr << "Register index out of bounds in SRA" << endl;
		return;
	}
	int shiftAmount = registers[rs2];			   // RISC-V uses only the lower 5 bits for shift amount
	registers[rd] = registers[rs1] >> shiftAmount; // Arithmetic right shift
}

void CPU::SLTIU(int rd, int rs, int imm)
{
	if (rd < 0 || rd >= 32 || rs < 0 || rs >= 32)
	{
		cerr << "Register index out of bounds in SLTIU" << endl;
		return;
	}
	uint rs_val = static_cast<uint>(registers[rs]);
	uint imm_val = static_cast<uint>(imm);

	registers[rd] = (rs_val < imm_val) ? 1 : 0;
}

void CPU::LW(int rd, int rs1, int offset)
{
	if (rd < 0 || rd >= 32 || rs1 < 0 || rs1 >= 32)
	{
		cerr << "Register index out of bounds in LW" << endl;
		return;
	}
	int address = registers[rs1] + offset;
	if (address < 0 || address + 3 >= 4096)
	{
		cerr << "Memory access out of bounds in LW" << endl;
		return;
	}
	// Load 4 bytes from memory and combine them into a single integer
	registers[rd] = (dmemory[address] & 0xFF) |
					((dmemory[address + 1] & 0xFF) << 8) |
					((dmemory[address + 2] & 0xFF) << 16) |
					((dmemory[address + 3] & 0xFF) << 24);
}

void CPU::SW(int rs2, int rs1, int offset)
{
	if (rs2 < 0 || rs2 >= 32 || rs1 < 0 || rs1 >= 32)
	{
		cerr << "Register index out of bounds in SW" << endl;
		return;
	}
	int address = registers[rs1] + offset;
	if (address < 0 || address + 3 >= 4096)
	{
		cerr << "Memory access out of bounds in SW" << endl;
		return;
	}
	// Store the integer into 4 bytes in memory
	dmemory[address] = registers[rs2] & 0xFF;
	dmemory[address + 1] = (registers[rs2] >> 8) & 0xFF;
	dmemory[address + 2] = (registers[rs2] >> 16) & 0xFF;
	dmemory[address + 3] = (registers[rs2] >> 24) & 0xFF;
}

void CPU::LBU(int rd, int rs1, int offset)
{
	if (rd < 0 || rd >= 32 || rs1 < 0 || rs1 >= 32)
	{
		cerr << "Register index out of bounds in LBU" << endl;
		return;
	}
	int address = registers[rs1] + offset;
	if (address < 0 || address >= 4096)
	{
		cerr << "Memory access out of bounds in LBU" << endl;
		return;
	}
	// Load a single byte from memory and zero-extend it to an integer
	registers[rd] = dmemory[address] & 0xFF;
}

void CPU::BNE(int rs1, int rs2, int imm)
{
	if (rs1 < 0 || rs1 >= 32 || rs2 < 0 || rs2 >= 32)
	{
		cerr << "Register index out of bounds in BNE" << endl;
		return;
	}
	if (registers[rs1] != registers[rs2])
	{
		PC += imm; // Branch by adding immediate to PC
	}
	else
	{
		PC++; // Increment PC normally if not branching
	}
}

void CPU::JALR(int rd, int rs1, int offset)
{
	registers[rd] = PC + 1;
	PC = (registers[rs1] + offset) & ~1;
}

void CPU::LUI(int rd, int imm)
{
	if (rd < 0 || rd >= 32)
	{
		cerr << "Register index out of bounds in LUI" << endl;
		return;
	}
	registers[rd] = imm << 12; // Load upper immediate
}

InstructionInfo CPU::decode(bitset<32> instr)
{
	InstructionInfo info;
	uint32_t opcode = instr.to_ulong() & 0x7F;

	uint8_t funct3, funct7;

	switch (opcode)
	{
	case 0b0110011:								 // SRA, SUB, AND
		funct3 = (instr.to_ulong() >> 12) & 0x7; // bits 12-14

		info.read_register1 = (instr.to_ulong() >> 15) & 0x1F; // Get read_register1
		info.read_register2 = (instr.to_ulong() >> 20) & 0x1F; // Get read_register2
		info.write_register = (instr.to_ulong() >> 7) & 0x1F;	 // Get write_register
		switch (funct3)
		{
		case 0b101: // SRA
			// handle
			// R-type operation
			info.instruction_name = "SRA";

			break;
		case 0b000: // SUB
			// handle
			// R-type operation
			info.instruction_name = "SUB";
			break;
		case 0b111: // AND
			// handle
			// R-type operation
			info.instruction_name = "AND";
			break;
		}
		break;
	case 0b0010011: // ADDI, ORI, SLTIU
					// bits 12-14
		funct3 = (instr.to_ulong() >> 12) & 0x7;

		info.read_register1 = (instr.to_ulong() >> 15) & 0x1F; // Get read_register1
		info.write_register = (instr.to_ulong() >> 7) & 0x1F;	 // Get write_register
		int32_t imm = (instr.to_ulong() >> 20) & 0xFFF;		 // Get immediate

		if (imm & 0x800) {
			imm |= 0xFFFFF000;
		}
		info.immediate = imm;

		switch (funct3)
		{
		case 0b000: // ADDI
					// I-type operation
			info.instruction_name = "ADDI";
			break;
		case 0b110: // ORI
					// I-type operation
			info.instruction_name = "ORI";
			break;
		case 0b011: // SLTIU
					// I-type operation
			info.instruction_name = "SLTIU";
			break;
		}
		break;
	case 0b0000011: // LW, LBU
		funct3 = (instr.to_ulong() >> 12) & 0x7; // bits 12-14

		info.read_register1 = (instr.to_ulong() >> 15) & 0x1F;
		info.write_register = (instr.to_ulong() >> 7)& 0x1F;
		int32_t imm2 = (instr.to_ulong() >> 20) & 0xFFF;

		if (imm2 & 0x800) {
			imm2 |= 0xFFFFF000;
		}

		info.immediate = imm2;

		switch (funct3)
		{
			case 0b010:
				info.instruction_name = "LW";
			break;
			case 0b100:
				info.instruction_name = "LBU";
			break;
		}

		break;
	case 0b0100011: // SW, SH
					// funct3 can be used to differentiate between LW and LBU if needed
		funct3 = (instr.to_ulong() >> 12) & 0x7; // bits 12-14

		info.read_register1 = (instr.to_ulong() >> 15) & 0x1F;
		info.read_register2 = (instr.to_ulong() >> 20) & 0x1F;
		int32_t imm3 = ((instr.to_ulong()) >> 7) & 0x1F;
		imm3 |= ((instr.to_ulong() >> 25) & 0x7F) << 5;
		if (imm3 & 0x800) {
			imm3 |= 0xFFFFF000;
		}

		info.immediate = imm3;

		switch (funct3)
		{
			case 0b010:
				info.instruction_name = "SW";
			break;
			case 0b001:
				info.instruction_name = "SH";
		}
		break;

	case 0b1100011: // BNE
		info.instruction_name = "BNE";

		info.read_register1 = instr.to_ulong() >> 15 & 0x1F;
		info.read_register2 = instr.to_ulong() >> 20 & 0x1F;
		int32_t imm4 = ((instr.to_ulong()) >> 7) & 0x1E;
		imm4 |= (((instr.to_ulong()) >> 25) & 0x3F) << 5;
		imm4 |= (((instr.to_ulong()) >> 31) & 0x1) << 11;
		imm4 |= (((instr.to_ulong()) >> 7) & 0x1) << 12;
		if (imm4 & 0x1000) {
			imm4 |= 0xFFFFE000;
		}
		info.immediate = imm4;

		break;
	case 0b1100111: // JALR
		info.instruction_name = "JALR";

		info.read_register1 = (instr.to_ulong() >> 15) & 0x1F; // Get read_register1
		info.write_register = (instr.to_ulong() >> 7) & 0x1F;	 // Get write_register
		int32_t imm = (instr.to_ulong() >> 20) & 0xFFF;		 // Get immediate

		if (imm & 0x800) {
			imm |= 0xFFFFF000;
		}
		info.immediate = imm;

		break;	
	}

	return info;
}

int CPU::getRegister(int idx) const
{
	if (idx < 0 || idx >= 32)
	{
		cerr << "Register index out of bounds in getRegister" << endl;
		return 0;
	}
	return registers[idx];
}

void CPU::setRegister(int idx, int value)
{
	if (idx < 0 || idx >= 32)
	{
		cerr << "Register index out of bounds in setRegister" << endl;
		return;
	}
	registers[idx] = value;
}
// Add other functions here ...
