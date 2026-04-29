#include <stdio.h>
#include <stdlib.h>

/**
 * Reads N bytes from a file into a dynamically allocated buffer.
 *
 * @param filename  Path to the file to read from
 * @param n         Number of bytes to read
 * @param bytes_read  Output parameter: actual number of bytes read
 * @return          Pointer to allocated buffer containing the data,
 *                  or NULL on failure. Caller must free() the buffer.
 */
char *read_n_bytes(const char *filename, size_t n, size_t *bytes_read) {
    if (!filename || n == 0 || !bytes_read) {
        return NULL;
    }

    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("fopen");
        return NULL;
    }

    char *buffer = malloc(n + 1); // +1 for optional null terminator
    if (!buffer) {
        perror("malloc");
        fclose(fp);
        return NULL;
    }

    *bytes_read = fread(buffer, 1, n, fp);

    if (*bytes_read < n && ferror(fp)) {
        perror("fread");
        free(buffer);
        fclose(fp);
        return NULL;
    }

    buffer[*bytes_read] = '\0'; // null-terminate for safe string use
    fclose(fp);
    return buffer;
}

int main(void) {
    const char *filename = "example.txt";
    size_t n;

    printf("Enter number of bytes to read: ");
    if (scanf("%zu", &n) != 1 || n == 0) {
        fprintf(stderr, "Invalid input.\n");
        return 1;
    }

    size_t bytes_read = 0;
    char *buf = read_n_bytes(filename, n, &bytes_read);
    if (!buf) {
        fprintf(stderr, "Failed to read from file.\n");
        return 1;
    }

    printf("Requested: %zu bytes | Read: %zu bytes\n", n, bytes_read);
    printf("Data: %s\n", buf);

    free(buf);
    return 0;
}