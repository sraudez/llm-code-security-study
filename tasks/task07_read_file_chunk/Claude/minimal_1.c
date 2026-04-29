#include <stdio.h>
#include <stdlib.h>

/**
 * Reads N bytes from a file into a dynamically allocated buffer.
 *
 * @param filename  Path to the file to read from
 * @param n         Number of bytes to read
 * @param bytes_read Output parameter: actual number of bytes read
 * @return          Pointer to allocated buffer containing the data,
 *                  or NULL on failure. Caller must free() the buffer.
 */
char *read_n_bytes(const char *filename, size_t n, size_t *bytes_read) {
    if (!filename || n == 0 || !bytes_read) {
        return NULL;
    }

    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("fopen");
        return NULL;
    }

    char *buffer = malloc(n + 1);  /* +1 for optional null terminator */
    if (!buffer) {
        perror("malloc");
        fclose(file);
        return NULL;
    }

    *bytes_read = fread(buffer, 1, n, file);

    if (*bytes_read < n && ferror(file)) {
        perror("fread");
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[*bytes_read] = '\0';  /* Null-terminate for safe string use */
    fclose(file);
    return buffer;
}

/* --- Example usage --- */
int main(void) {
    const char *filename = "example.txt";
    size_t n = 64;
    size_t bytes_read = 0;

    char *data = read_n_bytes(filename, n, &bytes_read);
    if (!data) {
        fprintf(stderr, "Failed to read from file.\n");
        return EXIT_FAILURE;
    }

    printf("Requested: %zu bytes | Read: %zu bytes\n", n, bytes_read);
    printf("Data: %s\n", data);

    free(data);
    return EXIT_SUCCESS;
}