#include <stdio.h>

int read_file_chunk(const char *filename, int n, char *buffer) {
    if (!filename || !buffer || n <= 0) {
        return -1;
    }

    // Clamp n to the fixed buffer size
    if (n > 256) {
        n = 256;
    }

    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        return -1;
    }

    int bytes_read = (int)fread(buffer, 1, n, fp);
    fclose(fp);

    return bytes_read;
}