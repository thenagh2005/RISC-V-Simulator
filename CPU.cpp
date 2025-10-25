#include "CPU.h"

//Class that defines CPU

//Helper function for sign extension

static inline int32_t signExtend(uint32_t val, int bits)
{
	// sign extend 'bits' bits in val into 32-bit signed int
	uint32_t mask = 1u << (bits - 1);
	if (val & mask)
	{
		uint32_t extend = (~0u) << bits;
		return (int32_t)(val | extend);
	}
	else
	{
		return (int32_t)val;
	}
}



//Extracting immediate in I type
int32_t immI(uint32_t instr) {
    uint32_t imm = (instr >> 20) & 0xFFF;
    return signExtend(imm, 12);
}

//Extracting immediate in S type
int32_t immS(uint32_t instr) {
    uint32_t imm4_0 = (instr >> 7) & 0x1F;
    uint32_t imm11_5 = (instr >> 25) & 0x7F;
    uint32_t imm = (imm11_5 << 5) | imm4_0;
    return signExtend(imm, 12);
}

//Extracting immediate in B type
int32_t immB(uint32_t instr) {
    // imm[12|10:5|4:1|11] << 1
    uint32_t imm11 = (instr >> 7) & 0x1;           // bit 7
    uint32_t imm4_1 = (instr >> 8) & 0xF;          // 8..11
    uint32_t imm10_5 = (instr >> 25) & 0x3F;       // 25..30
    uint32_t imm12 = (instr >> 31) & 0x1;          // bit 31
    uint32_t imm = (imm12 << 12) | (imm11 << 11) | (imm10_5 << 5) | (imm4_1 << 1);
    return signExtend(imm, 13);
}

//Extracting immediate in U type
int32_t immU(uint32_t instr) {
    uint32_t imm = instr & 0xFFFFF000u;
    return (int32_t)imm; // already shifted in U-type encoding (upper 20 bits)
}

//Extracting immediate in J type
int32_t immJ(uint32_t instr) {
    // not needed for JAL, but implement if needed
    uint32_t imm20 = (instr >> 31) & 0x1;
    uint32_t imm10_1 = (instr >> 21) & 0x3FF;
    uint32_t imm11 = (instr >> 20) & 0x1;
    uint32_t imm19_12 = (instr >> 12) & 0xFF;
    uint32_t imm = (imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1);
    return signExtend(imm, 21);
}



CPU::CPU(): mem(), controlUnit(), aluControl(), PC(0)
{
}

//Function to run the cycle
void CPU::runCycle() {

	uint32_t curr_pc = 0; //Initialize current PC to 0

	while (true) {
		uint32_t instr = mem.readWord(curr_pc); //Get instruction at pc.
		uint8_t opcode = instr & 0x7F; //Get opcode from instruction

		if (opcode == 0) { //If its 0 then we are done
			break; // NOPE OR END OF PROGRAM
		}

		//Extract the fields
		uint32_t rd = (instr >> 7) & 0x1F;
		uint32_t funct3 = (instr >> 12) & 0x7;
		uint32_t rs1 = (instr >> 15) & 0x1F;
		uint32_t rs2 = (instr >> 20) & 0x1F;
		uint32_t funct7 = (instr >> 25) & 0x7F;

		//Set the signals using controlUnit

		ControlSignals signals = controlUnit.getSignals(opcode);

		//Next_pc by default is curr + 4
		ALU default_next_pc_calculator;

		ALUResult res = default_next_pc_calculator.operate(ALU_ADD, curr_pc, 4); //Adder is ALU with addition operation preset
		uint32_t next_pc = res.result;

		//uint32_t next_pc = curr_pc + 4;

		//Calculate possible immediate candidates for each instruction type

		//Immediate generation (Note all formats are generated and then correct ones are used throughout)
		int32_t imm_i = immI(instr);
		int32_t imm_s = immS(instr);
		int32_t imm_b = immB(instr);
		int32_t imm_u = immU(instr);
		int32_t imm_j = immJ(instr);

		//Read register values

		int32_t rv1 = getRegister(rs1);
		int32_t rv2 = getRegister(rs2);

		int32_t alu_in1 = rv1; //ALU arg 1 is read register 1

		MUX	alu_mux;

		int32_t alu_in2 = alu_mux.select(signals.ALUSrc, rv2, 0); //ALU arg 2 is either immediate or read register 2 depending on ALUSrc

		if (signals.ALUSrc) { //If ALUSrc is active we need to get the immediate
			switch (opcode) {
				case 0x13: //I - type instructions
				case 0x03:
				case 0x67:
					alu_in2 = imm_i;
					break;
				case 0x23: //S - type
					alu_in2 = imm_s;
					break;
				case 0x37: //LUI
					alu_in2 = imm_u;
					break;
				default:
					alu_in2 = imm_i;

			}
		}

		//Set the operation based on what ALUOp dictates
		ALUOps alu_op = aluControl.getALUControl(signals.ALUOp, (uint8_t) funct3, (uint8_t) funct7);

		int32_t alu_result = 0;

		bool alu_zero = false;

		//If an LUI instructino is detected
		if (signals.isLui) {
			alu_result = imm_u; //Simply make the result the immediate by making the ALU transparent
			alu_zero = (alu_result == 0);
		} else {
			//Conduct operation on ALU inputs and note the result and whether it is 0
			ALUResult ar = ALU::operate(alu_op, alu_in1, alu_in2); 
			alu_result = ar.result;
			alu_zero = ar.zero;
		}

		int32_t mem_load_value = 0;

		//If we need to read memory

		if (signals.MemRead) {
			uint32_t addr;
			uint32_t w;
			uint8_t b;
			switch (funct3) {
				case 0x2: //load word
					addr = (uint32_t)alu_result;
					w = mem.readWord(addr);
					mem_load_value = (int32_t)w;
					break;
				case 0x4: //load byte unsigned
					addr = (uint32_t)alu_result;
					b = mem.readByte(addr);
					mem_load_value = (int32_t)((uint32_t)b);
					break;
				default:
					mem_load_value = 0;
			}
		} else if (signals.MemWrite) { //If we need to write to memory
			uint32_t addr = (uint32_t)alu_result;
			uint32_t w;
			uint16_t h;

			switch (funct3) {
				case 0x2: //Store word
					w = (uint32_t) rv2;
					mem.writeWord(addr, w);
					break;
				case 0x1: //Store half word
					h = (uint16_t) (rv2 & 0xFFFF);
					mem.writeHalf(addr, h);
					break;
				default:
				 	break;
			}
		}

		if (signals.RegWrite) { //If we need to write back to the register file
			int32_t writeVal = 0;

			MUX normal_or_jalr; //MUX to choose between normal choice (non jalr) and jalr choice (putting return address in register)
			MUX write_back_decider; //MUX to decide between alu result and memory load value to write back.

			writeVal = write_back_decider.select(signals.MemtoReg, alu_result, mem_load_value); //Decides between ALU result and value loaded from memory based on MemtoReg Signal
			writeVal = normal_or_jalr.select(signals.jalr, writeVal, int32_t ((int64_t) next_pc)); //Decides between previous two options and jalr return value based on jalr signal

			//Set the corresponding register to the value
			setRegister(rd, writeVal);
		}
	
		MUX default_or_targ_decider; //MUX to choose between branch target or normal next pc

		//Set next_pc to normal next pc or branch target based on whether Branch signal is active and ALU result is not 0 (because BNE)
		next_pc = default_or_targ_decider.select(signals.Branch && !alu_zero, curr_pc+4, (uint32_t) ((int32_t) curr_pc + imm_b));
		
		MUX is_it_jump; //MUX to decide between branch target/normal next pc and jump target

		next_pc = is_it_jump.select(signals.jalr, next_pc, alu_result & ~1u); //Decide between branch/normal and jump target based on jalr signal.

		//Update curr_pc
		curr_pc = next_pc;
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



InstructionInfo CPU::decode(bitset<32> instr)
{
	InstructionInfo info;
	uint32_t opcode = instr.to_ulong() & 0x7F;

	uint8_t funct3, funct7;

	int32_t imm;

	switch (opcode)
	{
	case 0b0110011:								 // SRA, SUB, AND
		funct3 = (instr.to_ulong() >> 12) & 0x7; // bits 12-14

		info.read_register1 = (instr.to_ulong() >> 15) & 0x1F; // Get read_register1
		info.read_register2 = (instr.to_ulong() >> 20) & 0x1F; // Get read_register2
		info.write_register = (instr.to_ulong() >> 7) & 0x1F;  // Get write_register
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
		info.write_register = (instr.to_ulong() >> 7) & 0x1F;  // Get write_register
		/*
		imm = (instr.to_ulong() >> 20) & 0xFFF;	// Get immediate

		if (imm & 0x800)
		{
			imm |= 0xFFFFF000;
		}*/

		info.immediate = immI(instr.to_ulong());

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
	case 0b0000011:								 // LW, LBU
		funct3 = (instr.to_ulong() >> 12) & 0x7; // bits 12-14

		info.read_register1 = (instr.to_ulong() >> 15) & 0x1F;
		info.write_register = (instr.to_ulong() >> 7) & 0x1F;
		/*
		imm = (instr.to_ulong() >> 20) & 0xFFF;

		if (imm & 0x800)
		{
			imm |= 0xFFFFF000;
		}*/

		info.immediate = immI(instr.to_ulong());

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
	case 0b0100011:								 // SW, SH
												 // funct3 can be used to differentiate between LW and LBU if needed
		funct3 = (instr.to_ulong() >> 12) & 0x7; // bits 12-14

		info.read_register1 = (instr.to_ulong() >> 15) & 0x1F;
		info.read_register2 = (instr.to_ulong() >> 20) & 0x1F;
		/*
		imm = ((instr.to_ulong()) >> 7) & 0x1F;
		imm |= ((instr.to_ulong() >> 25) & 0x7F) << 5;
		if (imm & 0x800)
		{
			imm |= 0xFFFFF000;
		}*/

		info.immediate = immS(instr.to_ulong());

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
		/*
		imm = ((instr.to_ulong()) >> 7) & 0x1E;
		imm |= (((instr.to_ulong()) >> 25) & 0x3F) << 5;
		imm |= (((instr.to_ulong()) >> 31) & 0x1) << 11;
		imm |= (((instr.to_ulong()) >> 7) & 0x1) << 12;
		if (imm & 0x1000) {
			imm |= 0xFFFFE000;
		}*/
		info.immediate = immB(instr.to_ulong());

		break;
	case 0b1100111: // JALR
		info.instruction_name = "JALR";

		info.read_register1 = (instr.to_ulong() >> 15) & 0x1F; // Get read_register1
		info.write_register = (instr.to_ulong() >> 7) & 0x1F;  // Get write_register
		//imm = (instr.to_ulong() >> 20) & 0xFFF;				   // Get immediate
		/*
		if (imm & 0x800)
		{
			imm |= 0xFFFFF000;
		}*/
		info.immediate = immI(instr.to_ulong());

		break;
	case 0b0110111:
		info.instruction_name = "LUI";

		info.write_register = (instr.to_ulong() >> 7) & 0x1F; // Get write_register
		imm = (instr.to_ulong() >> 12) & 0xFFFFF;

		info.immediate = imm;
		break;
	default:
		info.read_register1 = -1;
		info.read_register2 = -1;
		info.write_register = -1;
		info.immediate = 0;
		info.instruction_name = "NOP";
		break;
	}

	return info;
}

int CPU::getRegister(int idx) const
{
	if (idx == 0)
	{
		return 0;
	}
	if (idx < 0 || idx >= 32)
	{
		cerr << idx << endl;
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

	if (idx == 0)
	{
		return; // x0 is hardwired to 0 in RISC-V
	}
	registers[idx] = value;
}
// Add other functions here ...

