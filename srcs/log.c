#include <ft_printf.h>
#include <symbols_set.h>
#include <config.h>

void log_error(const char* context, const char* message)
{
    config_t *config = get_config();
    ft_dprintf(STDERR_FILENO, "%s: %s: %s\n", config->program_name, context, message);
}