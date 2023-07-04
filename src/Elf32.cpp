#include "Elf32.h"

Elf32::~Elf32() {
	delete[] _phdr;
	delete[] _shdr;
	delete[] _symraw;
	_symt.clear();
}

int64_t Elf32::fetchSize(const ElfPart type) {
	switch (type) {
	case ElfPart::Header:
		return sizeof(elf32_shdr);
	case ElfPart::ProgramHeaderTable:
		return _header.e_phnum * sizeof(elf32_phdr);
	case ElfPart::SectionHeaderTable:
		return _header.e_shnum * sizeof(elf32_shdr);
	case ElfPart::SectionStrTable:
		if (_shdr) {
			return _shdr[_header.e_shstrndx].sh_size;
		}
		else {
			return -1;
		}break;
	case ElfPart::None:
		return -1;
	default:
		return -1;
	}

	return -1;
}

int64_t Elf32::fetchPosition(const ElfPart type) {
	switch (type) {
	case ElfPart::Header:
		return 0;
	case ElfPart::ProgramHeaderTable:
		return _header.e_phoff;
	case ElfPart::SectionHeaderTable:
		return _header.e_shoff;
	case ElfPart::SectionStrTable:
		if (_shdr) {
			return _shdr[_header.e_shstrndx].sh_offset;
		}
		else {
			return -1;
		}break;
	case ElfPart::None:
		return -1;
	default:
		return -1;
	}

	return -1;
}

void Elf32::parse(unsigned char* ptr, const ElfPart type) {
	switch (type) {
	case ElfPart::Header:
		return parseHeader(ptr);
	case ElfPart::ProgramHeaderTable:
		return parsePHT(ptr);
	case ElfPart::SectionHeaderTable:
		return parseSHT(ptr);
	case ElfPart::SectionStrTable:
		return parseSymStr(ptr);
	case ElfPart::None:
		return;
	default:
		return;
	}

	return;
}

void Elf32::parseHeader(unsigned char *ptr) {
	_header = *(elf32_hdr*)ptr;
	if (_order == nativeMachineOrder()) {
		return;
	}

	swapEndian(_header.e_type);
	swapEndian(_header.e_machine);
	swapEndian(_header.e_version);
	swapEndian(_header.e_entry);
	swapEndian(_header.e_phoff);
	swapEndian(_header.e_shoff);
	swapEndian(_header.e_flags);
	swapEndian(_header.e_ehsize);
	swapEndian(_header.e_phentsize);
	swapEndian(_header.e_phnum);
	swapEndian(_header.e_shentsize);
	swapEndian(_header.e_shnum);
	swapEndian(_header.e_shstrndx);
}

void Elf32::parsePHT(unsigned char* ptr) {
	_phdr = new elf32_phdr[_header.e_phnum];
	std::copy_n(ptr, sizeof(elf32_phdr) * _header.e_phnum, (unsigned char*)_phdr);
	if (_order == nativeMachineOrder()) {
		return;
	}

	for (int i = 0; i < _header.e_phnum; i++) {
		swapEndian(_phdr[i].p_type);
		swapEndian(_phdr[i].p_offset);
		swapEndian(_phdr[i].p_vaddr);
		swapEndian(_phdr[i].p_paddr);
		swapEndian(_phdr[i].p_filesz);
		swapEndian(_phdr[i].p_memsz);
		swapEndian(_phdr[i].p_flags);
		swapEndian(_phdr[i].p_align);
	}
}

void Elf32::parseSHT(unsigned char* ptr) {
	_shdr = new elf32_shdr[_header.e_shnum];
	std::copy_n(ptr, sizeof(elf32_shdr) * _header.e_shnum, (unsigned char*)_shdr);
	if (_order == nativeMachineOrder()) {
		return;
	}

	for (int i = 0; i < _header.e_phnum; i++) {
		swapEndian(_shdr[i].sh_name);
		swapEndian(_shdr[i].sh_type);
		swapEndian(_shdr[i].sh_flags);
		swapEndian(_shdr[i].sh_addr);
		swapEndian(_shdr[i].sh_offset);
		swapEndian(_shdr[i].sh_size);
		swapEndian(_shdr[i].sh_link);
		swapEndian(_shdr[i].sh_info);
		swapEndian(_shdr[i].sh_addralign);
		swapEndian(_shdr[i].sh_entsize);
	}
}

void Elf32::parseSymStr(unsigned char *ptr) {
	int sz = _shdr[_header.e_shstrndx].sh_size;
	_symraw = new char[sz];
	memcpy(_symraw, ptr, sz);
	--sz;
	char *cur = (char*)ptr + 1;
	_symt.emplace_back("");
	while (sz) {
		_symt.emplace_back(cur);
		sz -= _symt[_symt.size() - 1].size() + 1;
		cur += _symt[_symt.size() - 1].size() + 1;
	}

	return;
}

std::string to_string(const unsigned char ident[EI_NIDENT]) {
	char buffer[kBufferSize]{};
	std::string ret{};
	int left = leftPadding, right = rightPadding;
	ret += paddingStr("Magic Number:", left, right);
	for (int i = EI_MAG0; i < EI_NIDENT; i++) {
		sprintf(buffer, "0x%x ", ident[i]);
		ret += buffer;
	}

	ret += "\n";
	ret += paddingStr("File Identity:", left, right);
	for (int i = EI_MAG1; i <= EI_MAG3; i++) {
		sprintf(buffer, "%c", ident[i]);
		ret += buffer;
	}

	ret += "\n";
	ret += paddingStr("Arch Bit:", left, right) + ElfHdrBit[ident[EI_CLASS]] + "\n";
	ret += paddingStr("Arch Bit Order:", left, right) + ElfHdrOrder[ident[EI_DATA]] + "\n";
	ret += paddingStr("Version:", left, right) + std::to_string(ident[EI_VERSION]) + "\n";
	int value = ident[EI_OSABI] % 15;
	ret += paddingStr("OSAbi:", left, right) + ElfHdrAbi[value] + "\n";
	return ret;
}

std::string to_string(const elf32_hdr h) {
	std::string ret = to_string(h.e_ident);
	int left = leftPadding, right = rightPadding;
	ret += paddingStr("Type:", left, right) + ElfHdrType[h.e_type % 6] + "\n";
	ret += paddingStr("Machine:", left, right) + ElfHdrMachine[h.e_machine % 9] + "\n";
	ret += paddingStr("Version:", left, right) + ElfHdrVersion[h.e_version] + "\n";
	ret += paddingStr("Entry Point:", left, right) + tohex(h.e_entry) + "\n";
	ret += paddingStr("Program Hedaer Table Offset:", left, right) + std::to_string(h.e_phoff) + "\n";
	ret += paddingStr("Section Hedaer Table Offset:", left, right) + std::to_string(h.e_shoff) + "\n";
	ret += paddingStr("Flags:", left, right) + tohex(h.e_flags) + "\n";
	ret += paddingStr("Header Size:", left, right) + std::to_string(h.e_ehsize) + "(in bytes)\n";
	ret += paddingStr("Program Hedaer Size:", left, right) + std::to_string(h.e_phentsize) + "(in bytes)\n";
	ret += paddingStr("Program Header Number:", left, right) + std::to_string(h.e_phnum) + "\n";
	ret += paddingStr("Section Hedaer Size:", left, right) + std::to_string(h.e_shentsize) + "(in bytes)\n";
	ret += paddingStr("Section Header Number:", left, right) + std::to_string(h.e_shnum) + "\n";
	ret += paddingStr("String Table Seciton Index:", left, right) + std::to_string(h.e_shstrndx) + "\n";
	return ret;
}

std::string to_string(const elf32_phdr h) {
	std::string ret{};
	int left = 0, right = 12;
	ret += paddingStr(std::string(ElfPHdrType[h.p_type % 9]), 4, right);
	ret += paddingStr(std::to_string(h.p_offset), left, right);
	ret += paddingStr(tohex(h.p_vaddr), left, right * 1.5);
	ret += paddingStr(tohex(h.p_paddr), left, right * 1.5);
	ret += paddingStr(std::to_string(h.p_filesz), left, right);
	ret += paddingStr(std::to_string(h.p_memsz), left, right);
	ret += paddingStr(std::to_string(h.p_flags), left, right);
	ret += paddingStr(std::to_string(h.p_align), left, right) + "\n";
	return ret;
}

std::string to_string(const elf32_shdr h, const char *str) {
	std::string ret{};
	int left = 0, right = 8;
	ret += paddingStr(str + h.sh_name, 4, right * 2.5);
	ret += paddingStr(std::string(ElfSHdrType[h.sh_type % 20]), left, right * 2);
	ret += paddingStr(tohex(h.sh_addr), left, right);
	ret += paddingStr(std::to_string(h.sh_offset), left, right);
	ret += paddingStr(std::to_string(h.sh_size), left, right);
	ret += paddingStr(std::to_string(h.sh_entsize), left, right);
	ret += paddingStr(std::to_string(h.sh_flags), left, right);
	ret += paddingStr(std::to_string(h.sh_link), left, right);
	ret += paddingStr(std::to_string(h.sh_info), left, right);
	ret += paddingStr(std::to_string(h.sh_addralign), left, right) + "\n";
	return ret;
}

std::string to_string(const elf32_phdr* h, const int len) {
	std::string ret{};
	int left = 0, right = 12;
	ret += paddingStr("Type", left + 4, right);
	ret += paddingStr("Offset", left, right);
	ret += paddingStr("VirtAddr", left, right * 1.5);
	ret += paddingStr("PhyAddr", left, right * 1.5);
	ret += paddingStr("FileSz", left, right);
	ret += paddingStr("MemSz", left, right);
	ret += paddingStr("Flags", left, right);
	ret += paddingStr("Align", left, right) + "\n";
	for (int i = 0; i < len; i++) {
		ret += ::to_string(h[i]);
	}

	return ret;
}

std::string to_string(const elf32_shdr* h, const int len, const char *str) {
	std::string ret{};
	int left = 0, right = 8;
	ret += paddingStr("Name", 4, right * 2.5);
	ret += paddingStr("Type", left, right * 2);
	ret += paddingStr("Address", left, right);
	ret += paddingStr("Offset", left, right);
	ret += paddingStr("Size", left, right);
	ret += paddingStr("Entsize", left, right);
	ret += paddingStr("Flags", left, right);
	ret += paddingStr("Link", left, right);
	ret += paddingStr("Info", left, right);
	ret += paddingStr("Align", left, right) + "\n";
	for (int i = 0; i < len; i++) {
		ret += ::to_string(h[i], str);
	}

	return ret;
}


std::string Elf32::to_string() {
	std::string ret{};
	ret += "Elf Header:\n";
	ret += ::to_string(_header);
	ret += "\nSegment Header\n";
	ret += ::to_string(_phdr, _header.e_phnum);
	ret += "\nSection Header\n";
	ret += ::to_string(_shdr, _header.e_shnum, _symraw);
	return ret;
}