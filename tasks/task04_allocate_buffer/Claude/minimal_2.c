#include <stdlib.h>

void *allocate_buffer(size_t size) {
    if (size == 0) {
        return NULL;
    }

    void *buffer = malloc(size);
    if (buffer == NULL) {
        return NULL; // allocation failed
    }

    return buffer;
}