#ifndef __ELF_H__
#define __ELF_H__

#include <types.h>

#define ELF_MAGIC_NUMBER = {0x7F, 0x45, 0x4C, 0x46}
#define ELF_WORD_LEN_32 = 1
#define ELF_WORD_LEN_64 = 2
#define ELF_VERSION = 1

#define ELF_MACHINE_AARCH64 0xB7

typedef struct elf_header_struct {
    uint8_t magic_number[4];
    uint8_t word_len;               // 1 for 32 bit, 2 for 64 bit
    uint8_t endianness;             // 1 for little endian, 2 for big endian
    uint8_t version;                // should be 1
    uint8_t os_abi;
    uint8_t abi_version;
    uint8_t padding[7];             // reserved (should be 0x00)
    uint16_t e_type;
    uint16_t e_machine;             // (0xb7 for aarch64)
    uint32_t e_version;             // should be 1
    uint64_t e_entry;               // entry point for application
    uint64_t e_phoff;               // program header offset (usually 0x40 for 64bit)
    uint64_t e_shoff;               // section header offset
    uint32_t e_flags;               // depends on platform
    uint16_t e_ehsize;              // elf header size
    uint16_t e_phentsize;           // program header entry size (should be 0x38 for 64 bits)
    uint16_t e_phnum;               // contains number of entries in program header table
    uint16_t e_shentsize;           // Size of section header entry (should be 0x40 for 64 bits)
    uint16_t e_shnum;               // Contains number of entries in section header table
    uint16_t e_shstrndx;            // Contains index of the section header table entry that contains the section names
} elf_header_t;

typedef struct elf_program_header_struct {
    uint32_t p_type;                // Type of sector
    uint32_t p_flags;               // Segment dependent flags (0x1 - executable, 0x2 - writeable, 0x4 - readable)
    uint64_t p_offset;              // Offset of segment within file
    uint64_t p_vaddr;               // Virtual address of the segment within memory
    uint64_t p_paddr;               // Physical address (where relevant)
    uint64_t p_filesz;              // Size of the segment in file (in bytes)
    uint64_t p_memsz;               // Size of the segment in mem (in bytes)
    uint64_t p_align;               // 0 and 1 specify no alignment. Otherwise should be a positive, integral power of 2, with p_vaddr equating p_offset modulus p_align.
} elf_program_header_t;

typedef struct elf_section_header_struct {
    uint32_t sh_name;               // Offset to a string in the .shstrtab section with this sections name
    uint32_t sh_type;               // Type of section
    uint64_t sh_flags;              // Flags for section
    uint64_t sh_addr;               // Virtual address for section in memory (for sections that are loaded)
    uint64_t sh_offset;             // Offset for this section in file
    uint64_t sh_size;               // Size in bytes of section
    uint64_t sh_addralign;          // Alignment of address for section (must be power of 2)
    uint64_t sh_entsize;            // Contains the size, in bytes, of each entry, for sections that contain fixed-size entries. Otherwise, this field contains zero.
} elf_section_header_t;

#endif