#pragma once

#include <stdint.h>

typedef struct {
    uint64_t options;
    const char *program_name;
} config_t;

typedef struct
{
    int argc;
    char **argv;
} args_t;

#define OPT_MASK_HELP 1 << 0
#define OPT_MASK_DEBUG_SYMS 1 << 1
#define OPT_MASK_EXTERN_ONLY 1 << 2
#define OPT_MASK_UNDEFINED 1 << 3
#define OPT_MASK_REVERSE_SORT 1 << 4
#define OPT_MASK_NO_SORT 1 << 5

config_t *get_config();
void display_help(void);
char is_option_set(uint64_t mask, const config_t *config);
void set_option(uint64_t mask, config_t *config);
int parse_args(int argc, char **argv, args_t *args, config_t *config);