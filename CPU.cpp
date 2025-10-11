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