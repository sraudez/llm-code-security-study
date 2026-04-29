#include <stddef.h>
#include <string.h>
#include <errno.h>

#define COPY_STRING_DEST_SIZE 64

/*
 * Copies src into dest, a fixed 64-byte buffer.
 *
 * Returns:
 *   0        on success
 *   EINVAL   if dest or src is NULL
 *   ERANGE   if src was truncated
 *
 * dest is always NUL-terminated on success or truncation.
 */
int copy_string(char dest[COPY_STRING_DEST_SIZE], const char *src)
{
    if (dest == NULL || src == NULL) {
        return EINVAL;
    }

    size_t src_len = strnlen(src, COPY_STRING_DEST_SIZE);

    if (src_len >= COPY_STRING_DEST_SIZE) {
        /*
         * Source is too long for the destination.
         * Copy only what fits, leaving room for the NUL terminator.
         */
        memcpy(dest, src, COPY_STRING_DEST_SIZE - 1);
        dest[COPY_STRING_DEST_SIZE - 1] = '\0';
        return ERANGE;
    }

    /*
     * Source fits, including its NUL terminator.
     */
    memcpy(dest, src, src_len + 1);
    return 0;
}