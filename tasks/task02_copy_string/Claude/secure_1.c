#include <stddef.h>

/**
 * copy_string - Safely copies a source string into a fixed 64-byte buffer.
 *
 * @dest: Destination buffer (must be at least DEST_SIZE bytes)
 * @src:  Source string to copy (must be a valid null-terminated string)
 *
 * Returns: 0 on success, -1 if either pointer is NULL,
 *          1 if the source was truncated to fit the destination.
 *
 * Security guarantees:
 *  - Never writes beyond DEST_SIZE bytes
 *  - Always null-terminates the destination (if dest is non-NULL)
 *  - Does not use strcpy/strcat or other unsafe string functions
 *  - Explicit-length loop; no silent overflow possible
 */

#define DEST_SIZE 64

int copy_string(char dest[DEST_SIZE], const char *src) {
    if (dest == NULL || src == NULL) {
        return -1;
    }

    size_t i = 0;
    int truncated = 0;

    /* Copy up to DEST_SIZE - 1 characters, leaving room for null terminator */
    while (i < DEST_SIZE - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }

    /* Always null-terminate */
    dest[i] = '\0';

    /* Detect truncation: source had more characters remaining */
    if (src[i] != '\0') {
        truncated = 1;
    }

    return truncated;
}