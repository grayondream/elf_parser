#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <cstdint>
#include <cstdio>
#include <string_view>
#include "elf.h"
#include <memory>
#include "ElfBase.h"
using FileLenType = int64_t;

class ElfBase;
class ElfParser {	
public:
	ElfParser(const std::string_view &file);
	Error init();
	Error parse();

	std::string to_string() { return _elf->to_string(); }

private:
    Error identity();
	
private:
	Error readbuf(void *ptr, const FileLenType sz);
	Error read();
	Error seek(const FileLenType sz);
	FileLenType size() { return _size - _cur; }
	Error resetio();

private:
	Error parse(const ElfPart type);

private:
	FileLenType _cur{};
	FileLenType _readlen{};
	FileLenType _size{};
	uint8_t *_ioBuffer{};
	std::string _file{};
	FILE *_fp{};
	BitOrder _order{};
	FileBit _bit{};

private:
	std::shared_ptr<ElfBase> _elf;
};