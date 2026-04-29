#include <stddef.h>

/**
 * copy_string - Safely copies a source string into a fixed 64-byte buffer.
 *
 * @param dest   Pointer to the 64-byte destination buffer.
 * @param src    Pointer to the null-terminated source string.
 *
 * @return       0 on success, -1 if either pointer is NULL,
 *               1 if the source was truncated to fit the buffer.
 *
 * Security guarantees:
 *  - Never writes more than DEST_SIZE bytes (including null terminator).
 *  - Always null-terminates dest when dest and src are non-NULL.
 *  - Does not rely on strcpy / strncpy (avoids missing-null-terminator pitfall).
 *  - Signals truncation via a distinct return code so callers can react.
 */

#define DEST_SIZE 64

int copy_string(char *dest, const char *src)
{
    /* 1. Reject NULL pointers immediately. */
    if (dest == NULL || src == NULL) {
        return -1;
    }

    const size_t max_chars = DEST_SIZE - 1;   /* reserve one byte for '\0' */
    size_t i = 0;
    int truncated = 0;

    /* 2. Copy byte-by-byte until end-of-string or buffer limit. */
    while (i < max_chars && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }

    /* 3. Always null-terminate. */
    dest[i] = '\0';

    /* 4. Detect truncation: src had more data than the buffer can hold. */
    if (src[i] != '\0') {
        truncated = 1;
    }

    return truncated;   /* 0 = full copy, 1 = truncated */
}