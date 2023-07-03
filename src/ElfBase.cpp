#include "ElfBase.h"

BitOrder nativeMachineOrder() {
	uint32_t v{ 0x12345678 };
	if((0xff & v) == 0x12) {
		return BitOrder::Msb;
	}else {
		return BitOrder::Lsb;
	}

	return BitOrder::None;
}

std::string paddingStr(const std::string &str, const int left, const int right) {
	std::string ret(left, ' ');
	if (str.size() >= right) return ret + str;
	return ret + str + std::string(right - str.size(), ' ');
}

