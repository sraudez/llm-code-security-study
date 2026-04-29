#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int read_n_bytes(const char *filename, size_t n, unsigned char **out_buffer, size_t *out_read)
{
    if (filename == NULL || out_buffer == NULL || out_read == NULL) {
        return -1;
    }

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return -1;
    }

    unsigned char *buffer = malloc(n);
    if (buffer == NULL && n != 0) {
        fclose(file);
        return -1;
    }

    size_t bytes_read = fread(buffer, 1, n, file);

    if (ferror(file)) {
        free(buffer);
        fclose(file);
        return -1;
    }

    fclose(file);

    *out_buffer = buffer;
    *out_read = bytes_read;

    return 0;
}