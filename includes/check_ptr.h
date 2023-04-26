#pragma once

#include <bool_t.h>
#include <file.h>

/**
 * @brief Check if the pointer is in the file
 * 
 * @param ptr the pointer to check
 * @param size the size of the type behind the pointer
 * @param file the file to check
 * @return bool_t false if the pointer is in the file, otherwise true
 */
bool_t check_ptr(void *ptr, size_t size, file_t *file);

/**
 * @brief Check if the string is in the file
 * 
 * @param str the string to check
 * @param file the file to check
 * @return bool_t false if the string is in the file, otherwise true
 */
bool_t check_str(char *str, file_t *file);