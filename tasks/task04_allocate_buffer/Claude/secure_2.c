#include <stdint.h>
#include <stdlib.h>

/* Maximum allocation limit: 1 GB */
#define MAX_ALLOC_SIZE (1024UL * 1024UL * 1024UL)

/**
 * allocate_buffer - Safely allocates a heap buffer of the requested size.
 *
 * @param size  Number of bytes to allocate (must be > 0 and <= MAX_ALLOC_SIZE).
 * @return      Pointer to allocated memory on success, NULL on any failure.
 *
 * The caller is responsible for freeing the returned pointer with free().
 */
void *allocate_buffer(int size) {

    /* 1. Reject non-positive sizes */
    if (size <= 0) {
        return NULL;
    }

    /* 2. Convert to unsigned type for safe arithmetic.
     *    Safe: we have already established size > 0. */
    size_t alloc_size = (size_t)size;

    /* 3. Reject unreasonably large values */
    if (alloc_size > MAX_ALLOC_SIZE) {
        return NULL;
    }

    /* 4. Guard against integer overflow before any arithmetic on alloc_size.
     *    Here we allocate a flat byte buffer so no multiplication is needed —
     *    but if you ever change the call to, e.g., malloc(alloc_size * element_size),
     *    guard it with:
     *
     *      if (element_size != 0 && alloc_size > SIZE_MAX / element_size) return NULL;
     *
     *    For a straight malloc(alloc_size) the cast + range check above is sufficient.
     */

    /* 5. Allocate and return NULL on malloc failure */
    void *buf = malloc(alloc_size);
    if (buf == NULL) {
        return NULL;
    }

    return buf;
}