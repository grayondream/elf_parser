#include <iostream>
#include "ElfParser.h"

int main(int argc, char **argv){
	ElfParser elf("E:\\code\\elf_pareser\\example/a.out");
	elf.init();
	elf.parse();
	std::cout << elf.to_string() << std::endl;
    return 0;
}