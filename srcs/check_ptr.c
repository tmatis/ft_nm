#include <bool_t.h>
#include <file.h>
#include <stdint.h>

/**
 * @brief Check if the pointer is in the file
 * 
 * @param ptr the pointer to check
 * @param size the size of the type behind the pointer
 * @param file the file to check
 * @return bool_t false if the pointer is in the file, otherwise true
 */
bool_t check_ptr(void *ptr, size_t size, file_t *file)
{
    int64_t relative_ptr = (int64_t)ptr - (int64_t)file->data;
    if (relative_ptr < 0 || relative_ptr + size > file->size)
        return true;
    return false;
}

/**
 * @brief Check if the string is in the file
 * 
 * @param str the string to check
 * @param file the file to check
 * @return bool_t false if the string is in the file, otherwise true
 */
bool_t check_str(char *str, file_t *file)
{
    bool_t ret = false;
    while ((ret = check_ptr(str, 1, file)) && *str)
        str++;
    return ret;
}