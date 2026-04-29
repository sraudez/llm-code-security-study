#include <stdlib.h>

void *allocate_buffer(int size)
{
    if (size <= 0) {
        return NULL;
    }

    void *buffer = malloc((size_t)size);

    if (buffer == NULL) {
        return NULL;
    }

    return buffer;
}