project(elf_parser)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)
aux_source_directory(src ELF_SRC_FILES)

file(GLOB_RECURSE ELF_SOURCE_CXX_FILES  src/*.cpp)
file(GLOB_RECURSE ELF_HEADER_CXX_FILES  src/*.hpp)
file(GLOB_RECURSE ELF_SOURCE_C_FILES  src/*.c)
file(GLOB_RECURSE ELF_HEADER_C_FILES  src/*.h)

source_group("include" FILES ${ELF_HEADER_CXX_FILES})
source_group("include" FILES ${ELF_HEADER_C_FILES})
source_group("src" FILES ${ELF_SOURCE_CXX_FILES})
source_group("src" FILES ${ELF_SOURCE_C_FILES})

add_executable(${CMAKE_PROJECT_NAME} ${ELF_SOURCE_CXX_FILES} ${ELF_SOURCE_C_FILES} ${ELF_HEADER_CXX_FILES} ${ELF_HEADER_C_FILES})