#pragma once

#include <unitypes.h>
#include <bool_t.h>

bool_t *get_is_program_big_endian(void);
unsigned long long read_endian(void *data, size_t size);