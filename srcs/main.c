#include <file.h>
#include <ft_printf.h>
#include <errno.h>
#include <elf.h>
#include <check_elf.h>
#include <handle_elf.h>
#include <config.h>
#include <ft_string.h>

#define DEFAULT_PATH "a.out"

static int handle_file(const char *file_name)
{
    file_t file = file_load(file_name);
    if (!file.data)
        return 1;

    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file.data;
    if (check_elf(ehdr, file_name))
    {
        file_close(&file);
        return 1;
    }

    // if 64 bits
    if (ehdr->e_ident[EI_CLASS] == ELFCLASS64)
    {
        if (handle_elf_64(&file, file_name, get_config()))
        {
            file_close(&file);
            return 1;
        }
    }
    // if 32 bits
    else
    {
        if (handle_elf_32(&file, file_name))
        {
            file_close(&file);
            return 1;
        }
    }
    file_close(&file);
    return 0;
}

int main(int argc, char **argv)
{
    config_t *config = get_config();
    args_t args;
    if (parse_args(argc, argv, &args, config))
    {
        display_help();
        return 1;
    }

    if (is_option_set(OPT_MASK_HELP, config))
    {
        display_help();
        return 0;
    }

    if (args.argc == 0)
    {
        if (handle_file(DEFAULT_PATH))
            return 1;
    }
    else
    {
        for (int i = 0; i < args.argc; i++)
        {
            if (args.argc > 1)
                ft_printf("\n%s:\n", args.argv[i]);
            if (handle_file(args.argv[i]))
                return 1;
        }
    }

    return 0;
}