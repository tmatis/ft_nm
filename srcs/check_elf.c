#include <elf.h>
#include <bool_t.h>
#include <log.h>
#include <ft_string.h>
#include <string.h>

static bool_t check_elf_header(Elf64_Ehdr *header)
{
    if (ft_memcmp(header->e_ident, ELFMAG, SELFMAG))
        return true;
    return false;
}

static bool_t check_elf_class(Elf64_Ehdr *header)
{
    if (header->e_ident[EI_CLASS] == ELFCLASS64)
        return false;
    else if (header->e_ident[EI_CLASS] == ELFCLASS32)
        return false;
    return true;
}

static bool_t check_elf_endian(Elf64_Ehdr *header)
{
    if (header->e_ident[EI_DATA] == ELFDATA2LSB)
        return false;
    return true;
}

static bool_t check_elf_version(Elf64_Ehdr *header)
{
    if (header->e_ident[EI_VERSION] == EV_CURRENT)
        return false;
    return true;
}

/**
 * @brief Check if the file is an correct elf file
 * 
 * @param header the elf header
 * @param filename the filename for error message
 * @return bool_t false if the file is an correct elf file, otherwise false
 */
bool_t check_elf(Elf64_Ehdr *header, const char *filename)
{
    if (check_elf_header(header))
    {
        log_error(filename, "file format not recognized");
        return true;
    }
    if (check_elf_class(header))
    {
        log_error(filename, "file class not recognized");
        return true;
    }
    if (check_elf_endian(header))
    {
        log_error(filename, "file endian not supported");
        return true;
    }
    if (check_elf_version(header))
    {
        log_error(filename, "elf version not supported");
        return true;
    }
    return false;
}