#pragma once
#include <stdint.h>
struct Memory {
	uint8_t *zp;
	uint8_t *cache;

	uint8_t *sram;
	uint8_t *uram;

	uint8_t *page;
	uint8_t *drom;

	uint8_t *srom;


	static Memory *memory;

	Memory();
	~Memory();

	uint16_t GetPageAddr(uint8_t window);

	uint8_t Read(uint16_t address);
	void Write(uint16_t address, uint8_t value);
	static Memory *getMemory();


};

