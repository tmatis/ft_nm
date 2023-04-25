#include <stdio.h>
#include <elf.h>
#include <string.h>
#include <unistd.h>
#include <file.h>
#include <string.h>
#include <errno.h>
#include <symbols_set.h>
#include <read_endian.h>

void write_node(symbol_t *symbol)
{
    if (!symbol->name || !symbol->name[0])
        return;
    if (symbol->value || symbol->type == 'T' || symbol->type == 't' || symbol->type == 'B' || symbol->type == 'b')
        printf("%016lx", symbol->value);
    else
        printf("%16s", "");
    printf(" %c %s\n", symbol->type, symbol->name);
}

char letter_from_symbol(Elf64_Sym *sym, Elf64_Shdr *shdr)
{
    char letter = '?';
    unsigned char st_info = read_endian(&sym->st_info, sizeof(sym->st_info));
    if (ELF64_ST_TYPE(st_info) == STT_FILE)
        return 0;
    Elf64_Section st_shndx = read_endian(&sym->st_shndx, sizeof(sym->st_shndx));
    Elf64_Word sh_type = read_endian(&shdr->sh_type, sizeof(shdr->sh_type));
    Elf64_Xword sh_flags = read_endian(&shdr->sh_flags, sizeof(shdr->sh_flags));
    
    if (ELF64_ST_BIND(st_info) == STB_WEAK)
    {
        letter = 'W';
        if (st_shndx == SHN_UNDEF)
            return 'w';
    }
    else if (st_shndx == SHN_UNDEF)
        letter = 'U';
    else if (ELF64_ST_TYPE(st_info) == STT_FUNC)
        letter = 'T';
    // is in .bss section it's B
    else if (sh_type == SHT_NOBITS)
        letter = 'B';

    // if in .rodata or .rodata1 section it's R
    else if (!(sh_flags & SHF_WRITE))
        letter = 'R';
    // if in .data or .data1 section it's D
    else if (sh_flags & SHF_WRITE)
        letter = 'D';

    if (letter != '?' && ELF64_ST_BIND(st_info) == STB_LOCAL)
        letter += 32;
    return letter;
}

// int main(void)
// {
//     symbol_t symbol_a = {0, 'T', "__gmon_start__"};
//     symbol_t symbol_b = {0, 'T', "_GLOBAL_OFFSET_TABLE_"};

//     printf("%d\n", compare_symbols(&symbol_a, &symbol_b));
// }

int main(int argc, char **argv)
{
    btree_symbol_t *symbols = NULL;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    file_t file = file_load(argv[1]);
    if (!file.data)
    {
        fprintf(stderr, "Could not load file %s: %s\n", argv[1], strerror(errno));
        return 1;
    }

    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file.data;
    if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0)
    {
        fprintf(stderr, "Not an ELF file\n");
        file_close(&file);
        return 1;
    }

    if (ehdr->e_ident[EI_CLASS] != ELFCLASS64)
    {
        fprintf(stderr, "Not a 64-bit ELF file\n");
        file_close(&file);
        return 1;
    }

    if (ehdr->e_ident[EI_DATA] == ELFDATA2MSB)
        *get_is_program_big_endian() = true;
    else if (ehdr->e_ident[EI_DATA] != ELFDATA2LSB)
    {
        fprintf(stderr, "Unknown endianness\n");
        file_close(&file);
        return 1;
    }
    Elf64_Off e_shoff = read_endian(&ehdr->e_shoff, sizeof(ehdr->e_shoff));
    Elf64_Shdr *shdr = (Elf64_Shdr *)(file.data + e_shoff);
    Elf64_Half e_shnum = read_endian(&ehdr->e_shnum, sizeof(ehdr->e_shnum));
    for (size_t i = 0; i < e_shnum; i++)
    {
        if (shdr[i].sh_type == SHT_SYMTAB)
        {
            Elf64_Off sh_offset = read_endian(&shdr[i].sh_offset, sizeof(shdr[i].sh_offset));
            Elf64_Sym *sym = (Elf64_Sym *)(file.data + sh_offset);
            for (size_t j = 0; j < shdr[i].sh_size / shdr[i].sh_entsize; j++)
            {
                char *name = (char *)(file.data + shdr[shdr[i].sh_link].sh_offset + sym[j].st_name);
                char letter = letter_from_symbol(&sym[j], &shdr[sym[j].st_shndx]);
                if (!letter)
                    continue;
                symbol_t s = {
                    .value = sym[j].st_value,
                    .type = letter,
                    .name = name,
                    .index = sym[j].st_shndx};
                btree_symbol_t_insert(&symbols, &s);
            }
        }
    }

    btree_symbol_t_foreach(symbols, write_node);
    btree_symbol_t_clear(&symbols, NULL);

    file_close(&file);
    return 0;
}