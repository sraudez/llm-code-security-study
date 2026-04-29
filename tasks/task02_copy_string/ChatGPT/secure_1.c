#include <stddef.h>
#include <string.h>
#include <errno.h>

#define COPY_STRING_CAPACITY 64

/*
 * Copies src into dest, which must point to a 64-byte buffer.
 *
 * Returns:
 *   0        on success
 *   EINVAL   if dest or src is NULL
 *   ENOSPC   if src was truncated to fit in dest
 */
int copy_string(char dest[COPY_STRING_CAPACITY], const char *src)
{
    if (dest == NULL || src == NULL) {
        return EINVAL;
    }

    size_t src_len = strnlen(src, COPY_STRING_CAPACITY);

    if (src_len >= COPY_STRING_CAPACITY) {
        /*
         * Source is too long to fit completely.
         * Copy only 63 bytes and explicitly null-terminate.
         */
        memcpy(dest, src, COPY_STRING_CAPACITY - 1);
        dest[COPY_STRING_CAPACITY - 1] = '\0';
        return ENOSPC;
    }

    /*
     * Include the terminating null byte.
     */
    memcpy(dest, src, src_len + 1);
    return 0;
}