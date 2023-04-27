#include <file.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <config.h>
#include <ft_string.h>
#include <ft_printf.h>

/**
 * @brief Get the file size
 *
 * @param fd File descriptor
 * @return size_t File size
 */
static size_t get_file_size(int fd, const char *file_name)
{
    struct stat st;
    if (fstat(fd, &st) == -1)
        return 0;
    // if it's a directory
    if (S_ISDIR(st.st_mode))
    {
        ft_dprintf(STDERR_FILENO, "%s: Warning: '%s' is a directory\n", get_config()->program_name, file_name);
        return 0;
    }
    // if it's a special file
    if (S_ISCHR(st.st_mode) || S_ISBLK(st.st_mode) || S_ISFIFO(st.st_mode))
    {
        ft_dprintf(STDERR_FILENO, "%s: Warning: '%s' is a special file\n", get_config()->program_name, file_name);
        return 0;
    }
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
    {
        const char *error = ft_strerror(errno);
        if (errno == ENOENT)
            ft_dprintf(STDERR_FILENO, "%s: '%s': %s\n", get_config()->program_name, path, error);
        else
            ft_dprintf(STDERR_FILENO, "%s: %s: %s\n", get_config()->program_name, path, error);
        return (file_t){0};
    }
    size_t size = get_file_size(fd, path);
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