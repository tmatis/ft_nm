#include <elf.h>
#include <file.h>
#include <symbols_set.h>
#include <check_ptr.h>
#include <log.h>
#include <ft_printf.h>
#include <ft_char.h>
#include <config.h>

static void write_node_64(symbol_t *symbol)
{
    if (!symbol->name || !symbol->name[0])
        return;
    char type = ft_tolower(symbol->type);
    if (symbol->value ||
        type == 't' ||
        symbol->type == 'b')
        ft_printf("%016lx", symbol->value);
    else
        ft_printf("%16s", "");
    ft_printf(" %c %s\n", symbol->type, symbol->name);
}

static char letter_from_symbol(Elf64_Sym *sym, Elf64_Shdr *shdr, config_t *config)
{
    char letter = '?';
    // if a file return 0
    if (ELF64_ST_TYPE(sym->st_info) == STT_FILE)
        return 0;

    if (is_option_set(OPT_MASK_EXTERN_ONLY, config) && ELF64_ST_BIND(sym->st_info) == STB_LOCAL)
        return 0;
    if (is_option_set(OPT_MASK_UNDEFINED, config) && sym->st_shndx != SHN_UNDEF)
        return 0;

    if (ELF64_ST_BIND(sym->st_info) == STB_WEAK)
    {
        letter = 'W';
        if (sym->st_shndx == SHN_UNDEF)
            return 'w';
    }
    else if (sym->st_shndx == SHN_UNDEF)
        letter = 'U';
    else if (ELF64_ST_TYPE(sym->st_info) == STT_FUNC)
        letter = 'T';
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

int handle_elf_64(file_t *file, const char *file_name, config_t *config)
{
    btree_symbol_t *symbols = NULL;
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file->data;
    Elf64_Shdr *shdr = (Elf64_Shdr *)(file->data + ehdr->e_shoff);
    if (check_ptr(shdr, sizeof(Elf64_Shdr) * ehdr->e_shnum, file))
    {
        log_error(file_name, "Invalid section header");
        return 1;
    }

    for (size_t i = 0; i < ehdr->e_shnum; i++)
    {
        if (shdr[i].sh_type == SHT_SYMTAB)
        {
            Elf64_Sym *sym = (Elf64_Sym *)(file->data + shdr[i].sh_offset);
            if (check_ptr(sym, sizeof(Elf64_Sym) * shdr[i].sh_size / shdr[i].sh_entsize, file))
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
                Elf64_Shdr *symbol_section = &shdr[sym[j].st_shndx];
                char letter = letter_from_symbol(&sym[j], symbol_section, config);
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

    if (!symbols)
    {
        log_error(file_name, "no symbols");
        return 1;
    }
    btree_symbol_t_foreach(symbols, write_node_64);
    btree_symbol_t_clear(&symbols, NULL);
    return 0;
}