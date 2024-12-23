#include <lib/elf/elf.h>
#include <utils/printf.h>
#include <scheduler/task.h>
#include <mm/paging.h>
#include <scheduler/scheduler.h>

sint8_t elf_load(elf_header_t* header) {
    printf("ELF Entry point: 0x%08x%08x\n program header num: %d\n program header offset: %d\n", header->e_entry >> 32, (header->e_entry << 32) >> 32, header->e_phnum, header->e_phoff);

    elf_program_header_t* header1 = (elf_program_header_t *) ((uint64_t) header + header->e_phoff);
    printf("program header type:%d\n program header flags:0x%08x\n", header1->p_type, header1->p_flags);

    void* program = (void *) ((uint64_t) header + header1->p_offset);
    return move_to_userspace(program, header1->p_memsz, 0, get_current_task());
}