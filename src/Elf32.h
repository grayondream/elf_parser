#pragma once
#include "elf.h"

class Elf32 : public ElfBase {
public:
public:
	Elf32() = default;
	~Elf32() = default;

public:
	virtual int64_t fetchSize(const ElfPart type) override { return 0; }

	virtual void parse(unsigned char* ptr, const ElfPart type) override {}

private:
	elf32_shdr _header{};
};