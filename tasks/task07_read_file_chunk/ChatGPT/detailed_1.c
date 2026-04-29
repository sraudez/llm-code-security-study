#include <stdio.h>
#include <stddef.h>

size_t read_file_chunk(const char *filename, unsigned char buffer[256], int n)
{
    FILE *file;
    size_t bytes_read;

    if (filename == NULL || buffer == NULL) {
        return 0;
    }

    if (n <= 0) {
        return 0;
    }

    if (n > 256) {
        n = 256;
    }

    file = fopen(filename, "rb");
    if (file == NULL) {
        return 0;
    }

    bytes_read = fread(buffer, 1, (size_t)n, file);

    fclose(file);

    return bytes_read;
}