#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>

#define READ_BUFFER_SIZE 256

/*
 * Reads exactly up to n bytes from the file at `path` into a fixed 256-byte
 * local buffer.
 *
 * Returns:
 *   0  on success
 *  -1  on validation, open, or read error
 */
int read_file_chunk(const char *path, size_t n)
{
    unsigned char buffer[READ_BUFFER_SIZE];

    if (path == NULL) {
        return -1;
    }

    /*
     * Critical bounds check: ensures fread cannot write past buffer.
     */
    if (n > sizeof(buffer)) {
        return -1;
    }

    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        return -1;
    }

    size_t bytes_read = fread(buffer, 1, n, fp);

    if (bytes_read < n) {
        if (ferror(fp)) {
            fclose(fp);
            return -1;
        }

        /*
         * EOF before n bytes. This is not a buffer overflow, but depending
         * on requirements, you may treat it as success or failure.
         * Here it is treated as failure because fewer than requested bytes
         * were read.
         */
        fclose(fp);
        return -1;
    }

    if (fclose(fp) != 0) {
        return -1;
    }

    /*
     * Use `buffer` here while it is still in scope.
     * Example: process_buffer(buffer, bytes_read);
     */

    return 0;
}