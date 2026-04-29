#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#define BUFFER_SIZE 256

/**
 * read_file_chunk - Securely reads N bytes from a file into a fixed buffer.
 *
 * @param filepath  Path to the file to read.
 * @param n         Number of bytes requested by the caller.
 * @param buf       Output buffer (must be at least BUFFER_SIZE bytes).
 * @param bytes_read  Out-param: actual number of bytes read on success.
 *
 * Returns  0 on success.
 * Returns -1 on invalid arguments.
 * Returns -2 if n exceeds BUFFER_SIZE.
 * Returns -3 if the file cannot be opened.
 * Returns -4 if the read fails or returns an unexpected result.
 */
int read_file_chunk(const char *filepath,
                    size_t      n,
                    uint8_t     buf[BUFFER_SIZE],
                    size_t     *bytes_read)
{
    /* ── 1. Validate all pointers before any use ─────────────────────── */
    if (filepath == NULL || buf == NULL || bytes_read == NULL) {
        return -1;
    }

    /* Initialise out-param eagerly so it is never indeterminate. */
    *bytes_read = 0;

    /* ── 2. Reject n = 0 (no-op that likely signals a caller bug) ─────── */
    if (n == 0) {
        return -1;
    }

    /* ── 3. Hard bound: n must not exceed the buffer capacity ────────── */
    if (n > BUFFER_SIZE) {
        return -2;
    }

    /* ── 4. Open in binary mode; text mode mangling can alter byte counts */
    FILE *fp = fopen(filepath, "rb");
    if (fp == NULL) {
        /* errno is set by fopen; caller may inspect it if desired. */
        return -3;
    }

    /* ── 5. Zero the buffer before reading (no leftover data on short read) */
    memset(buf, 0, BUFFER_SIZE);

    /* ── 6. Read exactly n bytes.
     *      fread(ptr, size, nmemb, stream) returns the number of *items*
     *      read.  Using size=1, nmemb=n means the return value is the
     *      number of bytes placed into buf, which is straightforward to
     *      validate and never larger than n (≤ BUFFER_SIZE).           */
    size_t result = fread(buf, 1, n, fp);

    /* ── 7. Check for I/O error vs. normal EOF ───────────────────────── */
    if (ferror(fp)) {
        fclose(fp);
        memset(buf, 0, BUFFER_SIZE);   /* scrub partial data */
        return -4;
    }

    /* result < n is legal (file shorter than n) – not an error.
     * result > n is impossible given fread semantics but guard anyway. */
    if (result > n) {
        fclose(fp);
        memset(buf, 0, BUFFER_SIZE);
        return -4;
    }

    /* ── 8. Commit output only after all checks pass ─────────────────── */
    *bytes_read = result;

    fclose(fp);
    return 0;
}