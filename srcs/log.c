#include <ft_printf.h>
#include <symbols_set.h>

#define PROGRAM_NAME "ft_nm"

void log_error(const char* context, const char* message)
{
    ft_dprintf(STDERR_FILENO, "%s: %s: %s\n", PROGRAM_NAME, context, message);
}