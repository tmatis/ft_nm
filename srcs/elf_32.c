#include <elf.h>
#include <file.h>
#include <symbols_set.h>
#include <check_ptr.h>
#include <log.h>
#include <ft_printf.h>

static void write_node_32(symbol_t *symbol)
{
    if (!symbol->name || !symbol->name[0])
        return;
    if (symbol->value || symbol->type == 'T' || symbol->type == 't' || symbol->type == 'B' || symbol->type == 'b')
        ft_printf("%08lx", symbol->value);
    else
        ft_printf("%08s", "");
    ft_printf(" %c %s\n", symbol->type, symbol->name);
}


static char letter_from_symbol(Elf32_Sym *sym, Elf32_Shdr *shdr)
{
    char letter = '?';
    // if a file return 0
    if (ELF32_ST_TYPE(sym->st_info) == STT_FILE)
        return 0;

    if (ELF32_ST_BIND(sym->st_info) == STB_WEAK)
    {
        letter = 'W';
        if (sym->st_shndx == SHN_UNDEF)
            return 'w';
    }

    else if (ELF32_ST_TYPE(sym->st_info) == STT_FUNC)
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

    if (letter != '?' && ELF32_ST_BIND(sym->st_info) == STB_LOCAL)
        letter += 32;
    return letter;
}

int handle_elf_32(file_t *file, const char *file_name)
{
    btree_symbol_t *symbols = NULL;
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)file->data;
    Elf32_Shdr *shdr = (Elf32_Shdr *)(file->data + ehdr->e_shoff);
    if (check_ptr(shdr, sizeof(Elf32_Shdr) * ehdr->e_shnum, file))
    {
        log_error(file_name, "Invalid section header");
        return 1;
    }

    for (size_t i = 0; i < ehdr->e_shnum; i++)
    {
        if (shdr[i].sh_type == SHT_SYMTAB)
        {
            Elf32_Sym *sym = (Elf32_Sym *)(file->data + shdr[i].sh_offset);
            if (check_ptr(sym, sizeof(Elf32_Sym) * shdr[i].sh_size / shdr[i].sh_entsize, file))
            {
                log_error(file_name, "Invalid symbol table");
                btree_symbol_t_clear(&symbols, NULL);
                return 1;
            }
            for (size_t j = 0; j < shdr[i].sh_size / shdr[i].sh_entsize; j++)
            {
                char *name = (char *)(file->data + shdr[shdr[i].sh_link].sh_offset + sym[j].st_name);
                if (check_str(name, file))
                {
                    log_error(file_name, "Invalid symbol name");
                    btree_symbol_t_clear(&symbols, NULL);
                    return 1;
                }
                Elf32_Shdr *symbol_section = &shdr[sym[j].st_shndx];
                char letter = letter_from_symbol(&sym[j], symbol_section);
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

    btree_symbol_t_foreach(symbols, write_node_32);
    btree_symbol_t_clear(&symbols, NULL);
    return 0;
}