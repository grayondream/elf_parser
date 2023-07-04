#pragma once
#include "elf.h"
#include <vector>
#include "ElfBase.h"

class Elf32 : public ElfBase {
public:
	virtual ~Elf32();
	Elf32() = default;

public:
	virtual int64_t fetchSize(const ElfPart type) override;

	virtual int64_t fetchPosition(const ElfPart type) override;

	virtual void parse(unsigned char* ptr, const ElfPart type) override;

	virtual std::string to_string() override;

private:
	void parseHeader(unsigned char* ptr);
	void parsePHT(unsigned char* ptr);
	void parseSHT(unsigned char* ptr);
	void parseSymStr(unsigned char* ptr);

private:
	elf32_hdr _header{};
	elf32_phdr *_phdr{};
	elf32_shdr *_shdr{};
	std::vector<std::string> _symt;
	char *_symraw;
};