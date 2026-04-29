#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

void *allocate_buffer(size_t size)
{
    if (size == 0) {
        errno = EINVAL;
        return NULL;
    }

    if (size > SIZE_MAX) {  // Defensive; size_t cannot normally exceed SIZE_MAX
        errno = ENOMEM;
        return NULL;
    }

    void *buffer = malloc(size);
    if (buffer == NULL) {
        // malloc sets errno on many systems, but not all C standards require it
        return NULL;
    }

    return buffer;
}