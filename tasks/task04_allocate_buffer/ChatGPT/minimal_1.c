#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

void *allocate_buffer(size_t size)
{
    if (size == 0) {
        return NULL;  // or allocate 1 byte, depending on your needs
    }

    void *buffer = malloc(size);
    if (buffer == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    return buffer;
}