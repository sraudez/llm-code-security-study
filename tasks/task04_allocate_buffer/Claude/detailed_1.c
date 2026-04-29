#include <stdlib.h>

void *allocate_buffer(int size) {
    if (size <= 0) return NULL;
    return malloc(size);
}