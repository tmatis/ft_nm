#include <stdint.h>
#include <unitypes.h>
#include <bool_t.h>
#include <stdio.h>

static bool_t is_computer_big_endian(void)
{
    static int is_big_endian = -1;
    if (is_big_endian == -1)
    {
        uint16_t value = 0x0102;
        uint8_t *bytes = (uint8_t *)&value;
        is_big_endian = bytes[0] == 0x01;
    }
    return is_big_endian;
}

bool_t *get_is_program_big_endian(void)
{
    static bool_t is_big_endian = 0;
    return &is_big_endian;
}

unsigned long long read_endian(void *data, size_t size)
{
    unsigned long long value = 0;
    uint8_t *bytes = (uint8_t *)&value;

    if (is_computer_big_endian() != *get_is_program_big_endian())
    {
        printf("Warning: reading endianess is not implemented yet\n");
        for (size_t i = 0; i < size; i++)
            bytes[i] = ((uint8_t *)data)[size - i - 1];
    }
    else
    {
        for (size_t i = 0; i < size; i++)
            bytes[i] = ((uint8_t *)data)[i];
    }
    return value;
}