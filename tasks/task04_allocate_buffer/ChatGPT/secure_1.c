#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#define MAX_BUFFER_SIZE (1024 * 1024 * 100)  /* 100 MiB limit; adjust as needed */

void *allocate_buffer(int size)
{
    size_t alloc_size;

    /*
     * Reject zero and negative sizes before converting to size_t.
     * A negative int converted to size_t would become a huge positive value.
     */
    if (size <= 0) {
        return NULL;
    }

    /*
     * Reject unreasonably large allocations.
     * This limit should be chosen based on your application’s needs.
     */
    if (size > MAX_BUFFER_SIZE) {
        return NULL;
    }

    /*
     * Check that conversion from int to size_t is safe.
     * This is defensive; with size > 0 and size <= MAX_BUFFER_SIZE,
     * it should already be safe on normal platforms.
     */
    if ((uintmax_t)size > (uintmax_t)SIZE_MAX) {
        return NULL;
    }

    alloc_size = (size_t)size;

    /*
     * Since we are allocating exactly `size` bytes, there is no multiplication
     * overflow risk here. If this later changes to count * element_size,
     * check that multiplication explicitly before malloc.
     */
    return malloc(alloc_size);
}