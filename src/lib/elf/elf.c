#include <lib/elf/elf.h>
#include <utils/printf.h>
#include <scheduler/task.h>
#include <mm/paging.h>
#include <scheduler/scheduler.h>
#include <config.h>

sint8_t elf_load(elf_header_t* header) {
    /* check elf header first */
    uint8_t mag[ELF_MAGIC_NUMBER_LEN] = ELF_MAGIC_NUMBER;
    for (uint8_t i = 0; i < ELF_MAGIC_NUMBER_LEN; i++) {
        if (header->magic_number[i] != mag[i]) return -1;
    }
    if (header->e_machine != ELF_MACHINE_AARCH64) return -1;
    if (header->word_len != ELF_WORD_LEN_64) return -1;

    #if EXTRA_DEBUG == DEBUG_ON
    printf("ELF Entry point: 0x%08x%08x\n program header num: %d\n program header offset: %d\n", header->e_entry >> 32, (header->e_entry << 32) >> 32, header->e_phnum, header->e_phoff);
    #endif

    elf_program_header_t* header1 = (elf_program_header_t *) ((uintptr_t) header + header->e_phoff);

    #if EXTRA_DEBUG == DEBUG_ON
    printf("program header type:%d\n program header flags:0x%08x\n", header1->p_type, header1->p_flags);
    #endif

    void* program = (void *) ((uintptr_t) header + header1->p_offset);
    return move_to_userspace(program, header1->p_memsz, 0, get_current_task());
}