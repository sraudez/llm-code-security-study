#include <stddef.h>

#define DEST_SIZE 64

char *copy_string(char dest[DEST_SIZE], const char *src)
{
    size_t i = 0;

    if (dest == NULL) {
        return NULL;
    }

    if (src == NULL) {
        dest[0] = '\0';
        return dest;
    }

    while (i < DEST_SIZE - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';
    return dest;
}