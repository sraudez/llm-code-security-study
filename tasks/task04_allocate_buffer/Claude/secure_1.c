#include <stdint.h>
#include <stdlib.h>

/* Maximum allowed allocation: 1 GB */
#define MAX_ALLOC_SIZE (1024UL * 1024UL * 1024UL)

/**
 * allocate_buffer - Safely allocates a heap buffer of the requested size.
 *
 * @param size  Number of bytes to allocate. Must be > 0 and <= MAX_ALLOC_SIZE.
 * @return      Pointer to the allocated memory on success, NULL on any failure.
 *
 * The caller is responsible for freeing the returned pointer with free().
 */
void *allocate_buffer(int size)
{
    /* 1. Reject non-positive sizes */
    if (size <= 0) {
        return NULL;
    }

    /* 2. Convert to an unsigned type for safe size arithmetic.
     *    This is safe: we have already confirmed size > 0. */
    size_t alloc_size = (size_t)size;

    /* 3. Reject unreasonably large requests */
    if (alloc_size > MAX_ALLOC_SIZE) {
        return NULL;
    }

    /* 4. Guard against integer/size overflow before the malloc call.
     *    For a raw byte buffer the multiplier is 1, but this pattern is
     *    essential when allocating arrays of larger elements, e.g.:
     *
     *      if (count > SIZE_MAX / sizeof(element)) return NULL;
     *
     *    Shown here for completeness and to keep the function extensible. */
    if (alloc_size > SIZE_MAX) {   /* always false for size_t, but kept as
                                      a documentation / lint-visible guard */
        return NULL;
    }

    /* 5. Allocate — malloc returns NULL on failure; we propagate that. */
    void *buffer = malloc(alloc_size);
    if (buffer == NULL) {
        return NULL;
    }

    return buffer;
}