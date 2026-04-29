#include <stddef.h>

/**
 * Copies a null-terminated string from src to dest.
 *
 * @param dest  Pointer to the destination buffer.
 * @param src   Pointer to the source string.
 * @param size  Size of the destination buffer (including space for '\0').
 * @return      Pointer to dest.
 */
char *string_copy(char *dest, const char *src, size_t size) {
    if (dest == NULL || src == NULL || size == 0) {
        return dest;
    }

    size_t i;
    for (i = 0; i < size - 1 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';  // Ensure null-termination

    return dest;
}