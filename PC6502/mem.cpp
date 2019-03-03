#include "mem.h"
Memory::Memory()
{
	zp = new uint8_t[0x80]();				// 0x0000..0x007F
	cache = new uint8_t[0x800000]();	// 0x0080..0x00FF Paged

	sram = new uint8_t[0xF00]();		// 0x0100..0x0FFF
	uram = new uint8_t[0x1000]();		// 0x1000..0x1FFF

	page = new uint8_t[0x40000000]();	// 0x2000..0xDFFF Paged
	drom = new uint8_t[0x10000000]();	// 0xE000..0xEFFF Paged

	srom = new uint8_t[0x1000]();		// 0xF000..0xFFFF
}

Memory::~Memory()
{
	delete[] zp;
	delete[] cache;

	delete[] sram;
	delete[] uram;

	delete[] page;
	delete[] drom;

	delete[] srom;
	//delete memory;
}

uint16_t Memory::GetPageAddr(uint8_t window)
{
	uint8_t LLPage = zp[0x74 + window * 4];
	uint8_t LHPage = zp[0x74 + window * 4 + 1];
	//uint8_t HLPage = progs[zp[0x70] + 2 + window * 4];
	//uint8_t HHPage = progs[zp[0x70] + 3 + window * 4];
	uint16_t PageAddr = LLPage | LHPage << 8; // | HLPage << 16 | HHPage << 24;
	return PageAddr;
}

uint8_t Memory::Read(uint16_t address)
{
	//std::cout << "WRITE: " << std::hex << address << ": " << std::hex << (int)value << "\n";
	uint8_t pg = (address & 0xF000) >> 12;
	uint8_t qrt = (pg - 2) & 0x3;
	uint8_t oct = pg >> 1;
	uint32_t pageAddr;
	uint8_t pageNum;
	address &= 0x0FFF;
	switch (pg)
	{
	case 0: // 0x0000
		if (address < 0x80) // Zero Page LO
		{
			return zp[address];
		}
		else if (address < 0x100) // Zero Page HI
		{
			return cache[((address - 0x80) | (zp[0x70] << 7) | zp[0x71] << 15)];
		}
		else
		{
			return sram[address - 0x100];
		}
	case 1: // 0x1000
		return uram[address];
	case 2: // 0x2000
	case 3: // 0x3000
	case 4: // 0x4000
	case 5: // 0x5000 Pile Window 1
		pageNum = 1;
		break;
	case 6: // 0x6000
	case 7: // 0x7000
	case 8: // 0x8000
	case 9: // 0x9000 Pile Window 2
		pageNum = 2;
		break;
	case 10: // 0xA000
	case 11: // 0xB000
	case 12: // 0xC000
	case 13: // 0xD000 Pile Window 3
		pageNum = 3;
		break;
	case 14: // 0xE000 Data ROM
		return drom[address | (zp[0x72] << 12) | (zp[0x73] << 20)];
	case 15: // 0xF000 System ROM
		return srom[address];
	}
	pageAddr = GetPageAddr(pageNum - 1);
	uint32_t finalAddress = (address | qrt << 12 | pageAddr << 14);
	return page[finalAddress];
}

void Memory::Write(uint16_t address, uint8_t value)
{
	//std::cout << "WRITE: " << std::hex << address << ": " << std::hex << (int)value << "\n";
	uint8_t pg = (address & 0xF000) >> 12;
	uint8_t qrt = (pg - 2) & 0x3;
	uint8_t oct = pg >> 1;
	uint32_t pageAddr;
	uint8_t pageNum;
	address &= 0x0FFF;
	switch (pg)
	{
	case 0: // 0x0000
		if (address < 0x80) // Zero Page LO
		{
			zp[address] = value;
			return;
		}
		else if (address < 0x100) // Zero Page HI
		{
			cache[((address - 0x80) | (zp[0x70] << 7) | zp[0x71] << 15)] = value;
			return;
		}
		else
		{
			sram[address - 0x100] = value;
			return;
		}
	case 1: // 0x1000
		uram[address] = value;
		return;
	case 2: // 0x2000
	case 3: // 0x3000
	case 4: // 0x4000
	case 5: // 0x5000 Pile Window 1
		pageNum = 1;
		break;
	case 6: // 0x6000
	case 7: // 0x7000
	case 8: // 0x8000
	case 9: // 0x9000 Pile Window 2
		pageNum = 2;
		break;
	case 10: // 0xA000
	case 11: // 0xB000
	case 12: // 0xC000
	case 13: // 0xD000 Pile Window 3
		pageNum = 3;
		break;
	case 14: // 0xE000 Data ROM
		drom[address | (zp[0x72] << 12) | (zp[0x73] << 20)] = value;
		return;
	case 15: // 0xF000 System ROM
		srom[address] = value;
		return;
	}
	pageAddr = GetPageAddr(pageNum - 1);
	uint32_t finalAddress = (address | qrt << 12 | pageAddr << 14);
	page[finalAddress] = value;
}

Memory *Memory::memory = nullptr;

Memory *Memory::getMemory()
{
	if (!memory) memory = new Memory();
	return memory;
}

