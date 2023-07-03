#pragma once
#include "elf.h"
#include "ElfBase.h"

class Elf64 : public ElfBase{
public:
	~Elf64() = default;
	Elf64() = default;

public:
	virtual int64_t fetchSize(const ElfPart type) override;

	virtual void parse(unsigned char* ptr, const ElfPart type) override;

	virtual std::string to_string() override;

private:
	void parserHeader(unsigned char* ptr);

private:
	elf64_hdr _header{};
};