#pragma once

#include <file.h>
#include <symbols_set.h>
#include <config.h>

int handle_elf_64(file_t *file, const char *file_name, config_t *config);
int handle_elf_32(file_t *file, const char *file_name, config_t *config);