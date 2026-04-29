#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int process_data(const uint8_t *input, size_t input_len, int use_buffer)
{
    uint8_t *buffer = NULL;
    int result = 0;

    if (input == NULL && input_len != 0) {
        return EINVAL;
    }

    if (input_len == 0) {
        return 0;
    }

    buffer = malloc(input_len);
    if (buffer == NULL) {
        return ENOMEM;
    }

    memcpy(buffer, input, input_len);

    /*
     * Use the buffer only while it is valid.
     * Never free it before all possible uses are complete.
     */
    if (use_buffer) {
        /*
         * Example conditional use:
         * compute a simple checksum-like value.
         */
        uint8_t checksum = 0;

        for (size_t i = 0; i < input_len; ++i) {
            checksum ^= buffer[i];
        }

        result = (int)checksum;
    }

    /*
     * Free exactly once, then immediately clear the pointer.
     * No code below this point dereferences buffer.
     */
    free(buffer);
    buffer = NULL;

    return result;
}