#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#define BUFFER_SIZE 256

/**
 * read_file_chunk - Securely reads up to N bytes from a file into a fixed buffer.
 *
 * @param  filepath   Path to the file to read.
 * @param  n          Number of bytes requested by the caller.
 * @param  out_buf    Output buffer (must be exactly BUFFER_SIZE bytes).
 * @param  out_bytes  Set to the number of bytes actually read on success.
 *
 * @return  0  on success
 *         -1  on invalid argument (NULL pointer, n == 0, or n > BUFFER_SIZE)
 *         -2  on file-open failure
 *         -3  on read failure (ferror)
 */
int read_file_chunk(const char *filepath,
                    size_t      n,
                    uint8_t     out_buf[BUFFER_SIZE],
                    size_t     *out_bytes)
{
    /* ── 1. Validate all arguments before touching anything ── */
    if (filepath == NULL || out_buf == NULL || out_bytes == NULL) {
        return -1;
    }

    /*
     * n must be at least 1 and must not exceed the hard buffer limit.
     * Using size_t (unsigned) means n can never be negative, but an
     * explicit zero-check guards against pointless empty reads.
     */
    if (n == 0 || n > BUFFER_SIZE) {
        return -1;
    }

    /* ── 2. Zero the output buffer before any read attempt ── */
    memset(out_buf, 0, BUFFER_SIZE);
    *out_bytes = 0;

    /* ── 3. Open the file in binary mode ── */
    FILE *fp = fopen(filepath, "rb");
    if (fp == NULL) {
        /* errno is set by fopen; caller may inspect it */
        return -2;
    }

    /* ── 4. Read exactly n bytes (or fewer at EOF) ── */
    /*
     * fread(ptr, size, count, stream) returns the number of *items* read.
     * With size=1 the item count equals the byte count, giving precise
     * control and making short-read detection straightforward.
     *
     * The read is explicitly capped at n, which has already been verified
     * to be <= BUFFER_SIZE, so this call cannot write past out_buf.
     */
    size_t bytes_read = fread(out_buf, 1, n, fp);

    /* ── 5. Distinguish EOF (ok) from a real I/O error ── */
    if (ferror(fp)) {
        fclose(fp);
        memset(out_buf, 0, BUFFER_SIZE); /* scrub on error */
        return -3;
    }

    /* ── 6. Clean up and report success ── */
    fclose(fp);
    *out_bytes = bytes_read;
    return 0;
}