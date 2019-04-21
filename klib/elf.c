/* Copyright (c) 2001 William L. Pitts
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are freely
 * permitted provided that the above copyright notice and this
 * paragraph and the following disclaimer are duplicated in all
 * such forms.
 *
 * This software is provided "AS IS" and without any express or
 * implied warranties, including, without limitation, the implied
 * warranties of merchantability and fitness for a particular
 * purpose.
 */

#include <elf.h>
#include <printf.h>

static bool valid_dest_addr(u32 addr, u32 size) {
    //  USER_RW   (RWX) : org = 0x95300000, len = 0x100000
    if (addr < 0x95300000u || addr >= 0x95300000u + 0x100000u) {
        return false;
    }
    if (addr + size > 0x95300000u + 0x100000u) {
        return false;
    }
    return true;
}


static char *_strtab = 0;
static void _find_strtab(u32 addr) {
    u32 i;
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)addr;
    Elf32_Shdr *shdr = (Elf32_Shdr *)(addr + ehdr->e_shoff);
    for (i = 0; i < ehdr->e_shnum; i++) {
        if (shdr->sh_type != SHT_STRTAB) {
            shdr++;
            continue;
        }
        _strtab = (char *) (addr + shdr->sh_offset);
        break;
    }
}

Elf32_Sym* _symtab = 0;
u32 _symnum = 0;
static void _find_symtab(u32 addr) {
    u32 i;
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)addr;
    Elf32_Shdr *shdr = (Elf32_Shdr *)(addr + ehdr->e_shoff);
    for (i = 0; i < ehdr->e_shnum; i++) {
        if (shdr->sh_type != SHT_SYMTAB) {
            shdr++;
            continue;
        }
        _symtab = (Elf32_Sym *) (addr + shdr->sh_offset);
        _symnum = shdr->sh_size / shdr->sh_entsize;
        break;
    }
}

u32 elf_load_from(u32 addr) {
    Elf32_Ehdr *ehdr;	/* Elf header structure pointer     */
    Elf32_Phdr *phdr;	/* Program header structure pointer */
    Elf32_Phdr proghdr;
    int i;

    ehdr = (Elf32_Ehdr *)addr;
    phdr = (Elf32_Phdr *)(addr + ehdr->e_phoff);
    if (!IS_ELF(*ehdr)) {
        printf("!IS_ELF(*ehdr)\n");
        return 0;
    }
    if (ehdr->e_type != ET_EXEC) {
        printf("ehdr->e_type != ET_EXEC\n");
        return 0;
    }

    /* Load each program header */
    for (i = 0; i < ehdr->e_phnum; ++i) {
        _memcpy(&proghdr, phdr, sizeof(Elf32_Phdr));
        if (proghdr.p_type != PT_LOAD) {
            ++phdr;
            continue;
        }
        if (!valid_dest_addr(proghdr.p_paddr, proghdr.p_filesz)) {
            printf("[ELF] Destination address [%08x] not available\n", proghdr.p_paddr);
            ++phdr;
            continue;
        }
        if (proghdr.p_filesz) {
            _memcpy((void *) proghdr.p_paddr, (void *) (addr + proghdr.p_offset), proghdr.p_filesz);
        }
        if (proghdr.p_filesz != proghdr.p_memsz) {
            _memset((void *) (proghdr.p_paddr + proghdr.p_filesz), 0x00, proghdr.p_memsz - proghdr.p_filesz);
        }
        ++phdr;
    }
    _find_strtab(addr);
    _find_symtab(addr);
    return ehdr->e_entry;
}


bool elf_find_symbol(char *name, u32* addr, u32 *size) {
    u32 i;
    if (_symtab == 0 || _strtab == 0) {
        panic("elf_find_symbol empty symtab or strtab\n");
    }
    Elf32_Sym *sym = _symtab;
    for (i = 0; i < _symnum; i++) {
        if (_strcmp(name, _strtab + sym->st_name) == 0) {
            *addr = sym->st_value;
            *size = sym->st_size;
            return true;
        }
        sym++;
    }
    return false;
}
