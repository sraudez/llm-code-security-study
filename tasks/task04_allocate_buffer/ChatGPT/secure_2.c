#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>

#define ALLOCATE_BUFFER_MAX_SIZE ((size_t)(1024U * 1024U * 16U)) /* 16 MiB */

void *allocate_buffer(int size)
{
    size_t n;

    /*
     * Reject zero and negative values before converting to size_t.
     * This prevents negative integers from becoming very large unsigned values.
     */
    if (size <= 0) {
        return NULL;
    }

    n = (size_t)size;

    /*
     * Reject values larger than the application-defined limit.
     * Choose this limit based on your program's real memory requirements.
     */
    if (n > ALLOCATE_BUFFER_MAX_SIZE) {
        return NULL;
    }

    /*
     * Overflow check before malloc.
     * For allocating raw bytes, no multiplication is needed, so the main
     * conversion risk was handled above. This check is kept as a defensive
     * pattern in case the allocation expression changes later.
     */
    if (n > SIZE_MAX) {
        return NULL;
    }

    errno = 0;
    return malloc(n);
}