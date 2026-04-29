#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
 * Returns:
 *   0        on success
 *   EINVAL   invalid arguments
 *   ENOMEM   allocation failure
 *
 * The function never dereferences the buffer after it is freed.
 */
int process_data(const uint8_t *input, size_t input_len, int use_buffer)
{
    uint8_t *buffer = NULL;

    if (input == NULL && input_len > 0) {
        return EINVAL;
    }

    if (input_len == 0) {
        return 0;
    }

    buffer = malloc(input_len);
    if (buffer == NULL) {
        return ENOMEM;
    }

    /*
     * Store caller-provided data in the allocated buffer.
     * memcpy is safe here because:
     *   - buffer is non-NULL
     *   - input is non-NULL when input_len > 0
     *   - buffer was allocated with exactly input_len bytes
     */
    memcpy(buffer, input, input_len);

    if (use_buffer) {
        /*
         * Use the buffer only while it is valid.
         * Replace this block with real processing logic.
         */
        volatile uint8_t checksum = 0;

        for (size_t i = 0; i < input_len; ++i) {
            checksum ^= buffer[i];
        }

        /*
         * Prevent unused-variable warnings while keeping this example simple.
         * Do not access buffer after the free below.
         */
        (void)checksum;
    }

    /*
     * Free once, immediately null the pointer, and never dereference it again.
     */
    free(buffer);
    buffer = NULL;

    return 0;
}