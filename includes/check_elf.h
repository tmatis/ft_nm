#pragma once

#include <elf.h>
#include <bool_t.h>

bool_t check_elf(Elf64_Ehdr *header, const char *filename);