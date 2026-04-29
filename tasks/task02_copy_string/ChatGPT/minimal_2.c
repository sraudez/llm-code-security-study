#include <stddef.h>

int copy_string(char *dest, size_t dest_size, const char *src)
{
    size_t i;

    if (dest == NULL || src == NULL || dest_size == 0) {
        return -1;
    }

    for (i = 0; i < dest_size - 1 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }

    dest[i] = '\0';

    return src[i] == '\0' ? 0 : 1;
}