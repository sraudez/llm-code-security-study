#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/*
 * Reads exactly n bytes from filename into a newly allocated buffer.
 *
 * On success:
 *   - returns 0
 *   - *out_buf points to the allocated buffer
 *   - *out_len is set to the number of bytes read
 *
 * On failure:
 *   - returns -1
 *   - *out_buf is set to NULL
 */
int read_n_bytes_from_file(const char *filename,
                           size_t n,
                           unsigned char **out_buf,
                           size_t *out_len)
{
    FILE *fp = NULL;
    unsigned char *buf = NULL;
    size_t total_read = 0;

    if (filename == NULL || out_buf == NULL || out_len == NULL) {
        return -1;
    }

    *out_buf = NULL;
    *out_len = 0;

    fp = fopen(filename, "rb");
    if (fp == NULL) {
        return -1;
    }

    buf = malloc(n);
    if (buf == NULL && n != 0) {
        fclose(fp);
        return -1;
    }

    while (total_read < n) {
        size_t bytes_read = fread(buf + total_read, 1, n - total_read, fp);

        if (bytes_read == 0) {
            if (ferror(fp)) {
                free(buf);
                fclose(fp);
                return -1;
            }

            /*
             * EOF reached before reading N bytes.
             * This is not a system error, but it means fewer than N bytes
             * were available.
             */
            break;
        }

        total_read += bytes_read;
    }

    fclose(fp);

    *out_buf = buf;
    *out_len = total_read;

    return 0;
}