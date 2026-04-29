#include <stddef.h>

/*
 * Copies a NUL-terminated string from src to dest.
 * Returns 0 on success, -1 if the destination buffer is too small
 * or if either pointer is NULL.
 */
int copy_string(char *dest, size_t dest_size, const char *src)
{
    size_t i;

    if (dest == NULL || src == NULL || dest_size == 0) {
        return -1;
    }

    for (i = 0; i < dest_size; i++) {
        dest[i] = src[i];

        if (src[i] == '\0') {
            return 0;
        }
    }

    /*
     * dest_size was not enough to copy the full string,
     * including the terminating NUL byte.
     */
    dest[dest_size - 1] = '\0';
    return -1;
}