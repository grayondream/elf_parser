#pragma once
#include <cstdint>
#include <string>
#include "elf.h"

constexpr int32_t kBufferSize = 1024;

enum class ElfPart : int32_t {
    None,
    Header,
	ProgramHeaderTable,
	SectionHeaderTable,
	SectionStrTable,
};
enum class Error : int32_t {
    None = 0,
    IO = -1,
    INVALID_DATA = -2,
    INVALID_PARAM = -3,
    FileEOF = -4,
    INVALID_OP = -5,
};

enum class FileBit : unsigned char {
    None = 0,
    Bit32 = 1,
    Bit64 = 2,
};

enum class BitOrder : unsigned char {
    None = 0,
    Lsb = 1,
    Msb = 2,
};

class ElfBase{
public:
	ElfBase() = default;
    virtual ~ElfBase() = default;

public:
    virtual int64_t fetchSize(const ElfPart type) = 0;

	virtual int64_t fetchPosition(const ElfPart type) = 0;

    virtual void parse(unsigned char* ptr, const ElfPart type) = 0;

    virtual void setOrder(const BitOrder order){ _order = order; }

	virtual std::string to_string() { return std::string{}; }

protected:
    BitOrder _order{};
};

template<class T>
T swapEndian(const T v) {
	static_assert(CHAR_BIT == 8, "char_bit is not 8");
	union{
		T u;
		unsigned char u8[sizeof(T)];
	}src, dst;
	src.u = v;
	for (int i = 0; i < sizeof(T); i++) {
		dst.u8[i] = src.u8[sizeof(T) - i - 1];
	}

	return dst.u;
}

template<class T>
std::string tohex(const T v) {
	char buff[16];
	sprintf(buff, "0x%llx", v);
	return buff;
}

BitOrder nativeMachineOrder();

std::string to_string(const unsigned char ident[EI_NIDENT]);

constexpr char *ElfHdrBit[] = { "None", "32bit","64bit" };
constexpr char *ElfHdrOrder[] = { "None", "LSB", "MSB" };
constexpr char *ElfHdrAbi[] = { "None", "Hewlett-Packard HP-UX", "NetBSD", "Linux", "Sun Solaris" , "AIX" , "IRIX" , "FreeBSD" , "Compaq TRU64 UNIX" , "Novell Modesto" , "Open BSD" , "Open VMS" , "Hewlett-Packard Non-Stop Kernel" , "Unknown" };
constexpr char *ElfHdrType[] = { "None", "Relocatable file", "Executable file", "Shared object file", "Core file", "Processor-specific" };
constexpr char *ElfHdrMachine[] = { "None", "AT&T WE 32100", "SPARC" , "Intel 80386" , "Motorola 68000" , "Motorola 88000" , "Intel 80860" , "MIPS RS3000", "Unknown" };
constexpr char *ElfHdrVersion[] = { "Invalid Version", "Current Version" };
constexpr int leftPadding = 4;
constexpr int rightPadding = 35;


constexpr char *ElfPHdrType[9] = { "PT_NULL", "PT_LOAD", "PT_DYNAMIC", "PT_INTERP", "PT_NOTE", "PT_SHLIB", "PT_PHDR", "PT_TLS", "Unknown" };
constexpr char *ElfSHdrType[20] = { "SHT_NULL", "SHT_PROGBITS", "SHT_SYMTAB", "SHT_STRTAB", "SHT_RELA", "SHT_HASH", "SHT_DYNAMIC", "SHT_NOTE", "SHT_NOBITS", "SHT_REL", "SHT_SHLIB", "SHT_DYNSYM", "Unknown", "Unknown", "SHT_INIT_ARRAY", "SHT_FINI_ARRAY", "SHT_PREINIT_ARRAY", "SHT_GROUP", "SHT_SYMTAB_SHNDX", "Unknown" };
std::string paddingStr(const std::string &str, const int left = 0, const int right = 10);

//64bit header
std::string to_string(const elf64_hdr h);
std::string to_string(const elf64_phdr* h, const int len);
std::string to_string(const elf64_shdr* h, const int len);
std::string to_string(const elf64_phdr h);
std::string to_string(const elf64_shdr h);

//32bit