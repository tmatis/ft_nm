#include <ft_char.h>
#include <symbols_set.h>
#include <elf.h>
#include <ft_printf.h>
#include <config.h>

static int compare_symbols_names(const char *a, const char *b)
{
    size_t i = 0;
    size_t j = 0;

    while (a[i] && b[j])
    {
        while (a[i] && !ft_isalnum(a[i]))
            i++;
        while (b[j] && !ft_isalnum(b[j]))
            j++;
        char a_letter = ft_tolower(a[i]);
        char b_letter = ft_tolower(b[j]);
        if (a_letter != b_letter)
            return a_letter - b_letter;
        i++;
        j++;
    }
    return a[i] - b[j];
}

static long int compare_symbols_index(Elf64_Section a, Elf64_Section b)
{
    return a - b;
}

static int compare_symbols_types(char a, char b)
{
    return a - b;
}

static long int compare_symbols_values(unsigned long a, unsigned long b)
{
    return a - b;
}

static int standard_compare_symbols(symbol_t *a, symbol_t *b)
{
    int cmp = compare_symbols_names(a->name, b->name);
    if (cmp)
        return cmp;
    cmp = compare_symbols_index(a->index, b->index);
    if (cmp)
        return cmp;
    cmp = compare_symbols_types(a->type, b->type);
    if (cmp)
        return cmp;
    return compare_symbols_values(a->value, b->value);
}

static int compare_symbols(symbol_t *a, symbol_t *b)
{
    config_t *config = get_config();
    if (is_option_set(OPT_MASK_REVERSE_SORT, config))
        return -standard_compare_symbols(a, b);
    if (is_option_set(OPT_MASK_NO_SORT, config))
        return 1;
    return standard_compare_symbols(a, b);
}

DEFINE_BTREE_FUNCTIONS(symbol_t, compare_symbols);