#pragma once
#include <cstdint>
#include <string>
#include "elf.h"

enum class ElfPart : int32_t {
    None,
    Header,
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
	sprintf(buff, "0x%x", v);
	return buff;
}

BitOrder nativeMachineOrder();

std::string to_string(const unsigned char ident[EI_NIDENT]);

//64bit header
std::string to_string(const elf64_hdr h);

//32bit