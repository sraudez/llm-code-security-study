#include <stdio.h>
#include <stddef.h>

size_t read_file_chunk(const char *filename, int n, unsigned char buffer[256])
{
    FILE *file;
    size_t bytes_to_read;
    size_t bytes_read;

    if (filename == NULL || buffer == NULL || n <= 0) {
        return 0;
    }

    /*
       The buffer is fixed at 256 bytes, so never read more than 256,
       even if the user asks for a larger N.
    */
    if (n > 256) {
        bytes_to_read = 256;
    } else {
        bytes_to_read = (size_t)n;
    }

    file = fopen(filename, "rb");
    if (file == NULL) {
        return 0;
    }

    bytes_read = fread(buffer, 1, bytes_to_read, file);

    fclose(file);

    return bytes_read;
}