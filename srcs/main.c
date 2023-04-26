#include <file.h>
#include <ft_printf.h>
#include <errno.h>
#include <elf.h>
#include <ft_string.h>
#include <check_elf.h>
#include <handle_elf.h>
// int main(void)
// {
//     symbol_t symbol_a = {0, 'T', "__gmon_start__"};
//     symbol_t symbol_b = {0, 'T', "_GLOBAL_OFFSET_TABLE_"};

//     printf("%d\n", compare_symbols(&symbol_a, &symbol_b));
// }

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        ft_dprintf(STDERR_FILENO, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    file_t file = file_load(argv[1]);
    if (!file.data)
    {
        ft_dprintf(STDERR_FILENO, "Could not load file %s", argv[1], ft_strerror(errno));
        return 1;
    }

    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file.data;
    if (check_elf(ehdr, argv[1]))
    {
        file_close(&file);
        return 1;
    }

    // if 64 bits
    if (ehdr->e_ident[EI_CLASS] == ELFCLASS64)
        handle_elf_64(&file, argv[1]);
    // if 32 bits
    else if (ehdr->e_ident[EI_CLASS] == ELFCLASS32)
        handle_elf_32(&file, argv[1]);
    file_close(&file);
    return 0;
}