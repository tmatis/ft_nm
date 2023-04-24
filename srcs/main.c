#include <stdio.h>
#include <elf.h>
#include <string.h>
#include <unistd.h>
#include <file.h>
#include <string.h>
#include <errno.h>
#include <symbols_set.h>

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
    // if a file return 0
    if (ELF64_ST_TYPE(sym->st_info) == STT_FILE)
        return 0;

    if (ELF64_ST_BIND(sym->st_info) == STB_WEAK)
    {
        letter = 'W';
        if (sym->st_shndx == SHN_UNDEF)
            return 'w';
    }

    else if (ELF64_ST_TYPE(sym->st_info) == STT_FUNC)
    {
        letter = 'T';
        if (sym->st_shndx == SHN_UNDEF)
            return 'U';
    }
    else if (sym->st_shndx == SHN_UNDEF)
        letter = 'U';
    // is in .bss section it's B
    else if (shdr->sh_type == SHT_NOBITS)
        letter = 'B';

    // if in .rodata or .rodata1 section it's R
    else if (!(shdr->sh_flags & SHF_WRITE))
        letter = 'R';
    // if in .data or .data1 section it's D
    else if (shdr->sh_flags & SHF_WRITE)
        letter = 'D';

    if (letter != '?' && ELF64_ST_BIND(sym->st_info) == STB_LOCAL)
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

    Elf64_Shdr *shdr = (Elf64_Shdr *)(file.data + ehdr->e_shoff);

    for (size_t i = 0; i < ehdr->e_shnum; i++)
    {
        if (shdr[i].sh_type == SHT_SYMTAB)
        {
            Elf64_Sym *sym = (Elf64_Sym *)(file.data + shdr[i].sh_offset);
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
                    .index = sym[j].st_shndx
                };
                btree_symbol_t_insert(&symbols, &s);
            }
        }
    }

    btree_symbol_t_foreach(symbols, write_node);

    file_close(&file);
    return 0;
}