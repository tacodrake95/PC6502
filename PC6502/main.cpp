#include <vector>
#include <fstream>
#include "mos6502.h"
#include "gfx.h"
#include "mem.h"



void Write(uint16_t address, uint8_t value)
{
	Memory::getMemory()->Write(address, value);
}

uint8_t Read(uint16_t address)
{
	uint8_t out = Memory::getMemory()->Read(address);
	//std::cout << "READ: " << std::hex << address << ": " << std::hex << (int)out << "\n";
	return out;
}

int main()
{
	Memory *mem = Memory::getMemory();
	
	Write(0xFFFC, 0x00);
	Write(0xFFFD, 0xF0);
	ifstream ROMfile("./ROM/kernel.rw", std::ios::in | std::ios::binary);
	ROMfile.read((char*)mem->srom, 0x1000);
	
	mos6502 cpu(Read, Write);
	
	gfx screen(3, 3);
	screen.GenTex(*(uint8_t*)NULL);// Memory::getMemory()->page[0]);
	//Memory::getMemory()->zp[0x60] = 0x18;
	while (!screen.shouldWindowClose())
	{
		cpu.Run(1000);
		screen.Render(*mem->page, mem->zp[0x60]);
	}
	return 0;
}