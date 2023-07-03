#pragma once
#include "elf.h"
#include "ElfBase.h"
#include <vector>

class Elf64 : public ElfBase{
public:
	virtual ~Elf64();
	Elf64() = default;

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
	elf64_hdr _header{};
	elf64_phdr *_phdr{};
	elf64_shdr *_shdr{};
	std::vector<std::string> _symt;
	char *_symraw;
};