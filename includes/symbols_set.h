#pragma once

#include <ft_btree.h>
#include <elf.h>

typedef struct
{
    long unsigned int value;
    char type;
    char *name;
    Elf64_Section index;
} symbol_t;

DEFINE_BTREE_HEADERS(symbol_t);