#include "ElfParser.h"
#include "Elf64.h"
#include "Elf32.h"

constexpr uint64_t kFileBufferMaxSize = 1024 * 1024 * 10;

ElfParser::ElfParser(const std::string_view &file) {
	_file = file.data();
}

Error ElfParser::init() {
	if (_file.empty()) return Error::INVALID_PARAM;
	if (_fp) return Error::None;
	_fp = fopen(_file.c_str(), "rb");
	if (!_fp) {
		printf("can not open file %s\n", _file.c_str());
		return Error::INVALID_PARAM;
	}

	_ioBuffer = new uint8_t[kFileBufferMaxSize];
	return Error::None;
}

Error ElfParser::parse() {
	Error error{};
	if (Error::None != ( error = identity())) {
		printf("the file maybe isn't a elf file\n");
		return error;
	}

	if (_bit == FileBit::Bit64) {
		_elf = std::make_shared<Elf64>();
	}else if (_bit == FileBit::Bit32) {
		_elf = std::make_shared<Elf32>();
	}else {
		printf("the file bit is wrong %d\n", static_cast<int>(_bit));
		return Error::INVALID_DATA;
	}

	_elf->setOrder(_order);
	
	parse(ElfPart::Header);
	parse(ElfPart::ProgramHeaderTable);
	parse(ElfPart::SectionHeaderTable);
	parse(ElfPart::SectionStrTable);
	return error;
}

Error ElfParser::seek(const FileLenType sz) {
	fseek(_fp, sz, SEEK_SET);
	_size = 0;
	_cur = 0;
	return read();
}

Error ElfParser::read() {
	const auto oldSz = _size;
	const auto requireSz = kFileBufferMaxSize - oldSz;
	std::copy_n((uint8_t*)_ioBuffer + _cur, _size - _cur, (uint8_t*)_ioBuffer);
	_cur = 0;

	const auto readlen = fread(_ioBuffer + oldSz, 1, requireSz, _fp);
	if (readlen < 0) {
		if (feof(_fp)) {
			printf("can not read more from file %lld, the file is eof", readlen);
			return Error::FileEOF;
		}

		printf("can not read more from file %lld", readlen);
		return Error::IO;
	}

	_size = oldSz + readlen;
	_readlen += readlen;
	return Error::None;
}

Error ElfParser::resetio() {
	fseek(_fp, 0, SEEK_SET);
	_size = 0;
	_cur = 0;
	return Error::None;
}

Error ElfParser::readbuf(void *ptr, const FileLenType sz) {
	Error error{};
	if (size() < sz) {
		if (Error::None != (error = read())) {
			return error;
		}

		if (size() < sz) {
			printf("the size is still smaller require, make sure your require is valid. requires size %lld, get size %lld\n", sz, size());
			return Error::INVALID_OP;
		}
	}

	std::copy_n((uint8_t*)_ioBuffer, sz, (uint8_t*)ptr);
	_cur += sz;
	return Error::None;
}

Error ElfParser::identity() {
	unsigned char ident[EI_NIDENT]{};
	int readlen{};
	if (Error::None != readbuf(ident, sizeof(unsigned char) * EI_NIDENT)) {
		return Error::IO;
	}

	if (ident[EI_MAG0] != ELFMAG0 || ident[EI_MAG1] != ELFMAG1 || ident[EI_MAG2] != ELFMAG2 || ident[EI_MAG3] != ELFMAG3) {
		printf("the first fource byte is not 0x7f E L f but %x %c %c %c\n", ident[EI_MAG0], ident[EI_MAG1], ident[EI_MAG2], ident[EI_MAG3]);
		return Error::INVALID_DATA;
	}

	_bit = static_cast<FileBit>(ident[4]);
	_order = static_cast<BitOrder>(ident[5]);
	resetio();
	return Error::None;
}

Error ElfParser::parse(const ElfPart type) {
	seek(_elf->fetchPosition(type));
	const auto sz = _elf->fetchSize(type);
	uint8_t *pdata = new uint8_t[sz];
	const auto error = readbuf(pdata, sz);
	if (error == Error::FileEOF) return Error::INVALID_OP;
	if (error != Error::None) return error;
	_elf->parse(pdata, type);
	return Error::None;
}
