#pragma once

#include <stddef.h>

typedef struct {
    void *data;
    size_t size;
} file_t;

/**
 * @brief Load a file into memory
 * 
 * @param path Path to the file
 * @return void* Pointer to the file data
 */
file_t file_load(const char *path);

/**
 * @brief Close the file
 * 
 * @param file Pointer to the file
 */
void file_close(file_t *file);