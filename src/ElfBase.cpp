#include "ElfBase.h"
constexpr int32_t kBufferSize = 1024;

BitOrder nativeMachineOrder() {
	uint32_t v{ 0x12345678 };
	if((0xff & v) == 0x12) {
		return BitOrder::Msb;
	}else {
		return BitOrder::Lsb;
	}

	return BitOrder::None;
}

std::string paddingStr(const std::string &str, const int left = 0, const int right = 10) {
	std::string ret(left, ' ');
	if (ret.size() >= right) return ret + str;
	return ret + str + std::string(right - str.size(), ' ');
}

constexpr char *ElfHdrBit[] = { "None", "32bit","64bit" };
constexpr char *ElfHdrOrder[] = { "None", "LSB", "MSB" };
constexpr char *ElfHdrAbi[] = { "None", "Hewlett-Packard HP-UX", "NetBSD", "Linux", "Sun Solaris" , "AIX" , "IRIX" , "FreeBSD" , "Compaq TRU64 UNIX" , "Novell Modesto" , "Open BSD" , "Open VMS" , "Hewlett-Packard Non-Stop Kernel" , "Unknown"};
constexpr char *ElfHdrType[] = { "None", "Relocatable file", "Executable file", "Shared object file", "Core file", "Processor-specific" };
constexpr char *ElfHdrMachine[] = {"None", "AT&T WE 32100", "SPARC" , "Intel 80386" , "Motorola 68000" , "Motorola 88000" , "Intel 80860" , "MIPS RS3000", "Unknown"};
constexpr char *ElfHdrVersion[] = { "Invalid Version", "Current Version" };
constexpr int leftPadding = 4;
constexpr int rightPadding = 35;

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

std::string to_string(const elf64_hdr h) {
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
	ret += paddingStr("String Table Seciton Index:", left, right) + std::to_string(h.e_shstrndx) + "(in bytes)\n";
	return ret;
}