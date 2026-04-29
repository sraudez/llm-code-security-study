#include <stddef.h>

#define DEST_BUFFER_SIZE 64

char *copy_string(char *dest, const char *src) {
    if (dest == NULL || src == NULL) {
        return dest;
    }

    size_t i;
    for (i = 0; i < DEST_BUFFER_SIZE - 1 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';

    return dest;
}