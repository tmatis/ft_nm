#include <config.h>
#include <ft_string.h>
#include <ft_printf.h>

typedef struct
{
    char letter;
    char *name;
    char *description;
    unsigned long int mask;
} option_t;

/**
 * @brief The list of options
 */
static const option_t options[] = {
    {'h', "help", "Display this information", OPT_MASK_HELP},
    {'a', "debug-syms", "Display debugger-only symbols", OPT_MASK_DEBUG_SYMS},
    {'g', "extern-only", "Display only external symbols", OPT_MASK_EXTERN_ONLY},
    {'u', "undefined-only", "Display only undefined symbols", OPT_MASK_UNDEFINED},
    {'r', "reverse-sort", "Reverse the sense of the sort", OPT_MASK_REVERSE_SORT},
    {'p', "no-sort", "Do not sort the symbols", OPT_MASK_NO_SORT},
    {0, NULL, NULL, 0}};

static const option_t *search_option_by_letter(char **arg, const option_t *options)
{
    for (int i = 0; options[i].letter != 0; i++)
    {
        if (options[i].letter == **arg)
        {
            (*arg)++;
            return &options[i];
        }
    }
    return NULL;
}

static const option_t *search_option_by_name(char **arg, const option_t *options)
{
    for (int i = 0; options[i].name != NULL; i++)
    {
        // compare till we reach \0 or =
        size_t j = 0;
        while (
            options[i].name[j] != '\0' && options[i].name[j] == (*arg)[j])
        {
            j++;
        }
        if (options[i].name[j] == '\0' && (*arg)[j] == '\0')
        {
            *arg += j;
            return &options[i];
        }
    }
    return NULL;
}

static int parse_arg_by_letter(char ***argv_pointer, int *argc_pointer, config_t *config)
{
    char *arg = (**argv_pointer) + 1; // we skip the '-'

    while (*arg != '\0')
    {
        const option_t *option = search_option_by_letter(&arg, options);
        if (option == NULL)
        {
            ft_printf("%s: invalid option -- '%c'\n", config->program_name, *arg);
            return 1;
        }
        set_option(option->mask, config);
    }
    (*argv_pointer)++;
    (*argc_pointer)--;
    return 0;
}

static int parse_arg_by_name(char ***argv_pointer, int *argc_pointer, config_t *config)
{
    char *arg = (**argv_pointer) + 2; // we skip the '--'
    const option_t *option = search_option_by_name(&arg, options);
    if (option == NULL)
    {
        ft_printf("%s: unrecognized option '--%s'\n", config->program_name, **argv_pointer);
        return 1;
    }
    set_option(option->mask, config);
    (*argv_pointer)++;
    (*argc_pointer)--;
    return 0;
}

int parse_args(int argc, char **argv, args_t *args, config_t *config)
{
    config->program_name = argv[0];
    argv++;
    argc--;
    while (argc && **argv == '-')
    {
        char *arg = *argv;
        arg++; // we skip the '-'
        if (*arg == '-')
        {
            if (parse_arg_by_name(&argv, &argc, config))
                return 1;
        }
        else
        {
            // if the arg is just '-' we do not count it as an option
            if (*arg == '\0')
                break;
            if (parse_arg_by_letter(&argv, &argc, config))
                return 1;
        }
    }
    args->argc = argc;
    args->argv = argv;
    return 0;
}

/**
 * @brief return the config in static memory
 *
 * @return config_t* the config
 */
config_t *get_config()
{
    static config_t config = {0, NULL};
    return &config;
}

void display_help(void)
{
    config_t *config = get_config();

    ft_printf("Usage: %s [option(s)] [file(s)]\n", config->program_name);
    ft_printf(" List symbols in [file(s)] (a.out by default).\n");
    ft_printf(" The options are:\n");
    for (int i = 0; options[i].letter != 0; i++)
        ft_printf(
            "  -%c, --%-16s %s\n",
            options[i].letter,
            options[i].name,
            options[i].description);
    ft_printf("%s: supported targets: elf64 elf32\n", config->program_name);
    ft_printf("%s by Theo Matis <tmatis@student.42.fr>\n", config->program_name);
}

/**
 * @brief Get if an option is set
 *
 * @param mask the mask of the option
 * @param config the config to check
 * @return char 1 if the option is set, 0 otherwise
 */
char is_option_set(uint64_t mask, const config_t *config)
{
    return (config->options & mask) != 0;
}

/**
 * @brief Set the option mask in the config
 *
 * @param mask The mask to set
 * @param config The config to update
 */
void set_option(uint64_t mask, config_t *config)
{
    config->options |= mask;
}