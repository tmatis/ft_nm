#include <file.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

/**
 * @brief Get the file size
 * 
 * @param fd File descriptor
 * @return size_t File size
 */
static size_t get_file_size(int fd)
{
    struct stat st;
    if (fstat(fd, &st) == -1)
        return 0;
    return st.st_size;
}

/**
 * @brief Load a file into memory
 *
 * @param path Path to the file
 * @return void* Pointer to the file data
 */
file_t file_load(const char *path)
{
    int fd = open(path, O_RDONLY);
    if (fd == -1)
        return (file_t){0};
    size_t size = get_file_size(fd);
    if (!size)
        return (file_t){0};
    char *data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED)
        return (file_t){0};
    close(fd);
    return (file_t){data, size};
}

/**
 * @brief Close the file
 *
 * @param file Pointer to the file
 */
void file_close(file_t *file)
{
    munmap(file->data, file->size);
    file->data = NULL;
    file->size = 0;
}