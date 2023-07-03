#include "Elf64.h"

int64_t Elf64::fetchSize(const ElfPart type) {
	switch (type) {
	case ElfPart::Header:
		return sizeof(elf64_shdr);
	case ElfPart::None:
		return -1;
	default:
		return -1;
	}

	return -1;
}

void Elf64::parse(unsigned char* ptr, const ElfPart type) {
	switch (type) {
	case ElfPart::Header:
		return parserHeader(ptr);
		return;
	case ElfPart::None:
		return;
	default:
		return;
	}

	return;
}

void Elf64::parserHeader(unsigned char *ptr) {
	_header = *(elf64_hdr*)ptr;
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

std::string Elf64::to_string() {
	std::string ret{};
	ret += "Elf Header:\n";
	ret += ::to_string(_header);

	return ret;
}