#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <errno.h>

#define READ_FILE_CHUNK_BUFFER_SIZE 256

/*
 * Reads up to n bytes from the file at path into a fixed 256-byte buffer.
 *
 * Returns:
 *   0  on success
 *  -1  on invalid input, open failure, read error, or short read
 */
int read_file_chunk(const char *path, size_t n)
{
    unsigned char buffer[READ_FILE_CHUNK_BUFFER_SIZE];

    if (path == NULL) {
        return -1;
    }

    /*
     * Critical bounds check:
     * n is size_t, so it cannot be negative.
     * Reject anything larger than the fixed buffer.
     */
    if (n > sizeof(buffer)) {
        return -1;
    }

    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return -1;
    }

    size_t bytes_read = fread(buffer, 1, n, file);

    if (ferror(file)) {
        fclose(file);
        return -1;
    }

    /*
     * Treat short reads as failure here.
     * If short reads are acceptable for your use case, return bytes_read
     * instead of failing.
     */
    if (bytes_read != n) {
        fclose(file);
        return -1;
    }

    if (fclose(file) != 0) {
        return -1;
    }

    /*
     * Use buffer here, or copy it to a caller-provided output buffer.
     * This placeholder prevents unused-variable warnings in some builds.
     */
    (void)buffer;

    return 0;
}