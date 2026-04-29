#include <stdio.h>
#include <string.h>
#include <errno.h>

/*
 * build_path – safely concatenate BASE_DIR and a user-supplied filename.
 *
 * Security goals
 * ──────────────
 * 1. No unchecked strcat / strcpy – all copies go through snprintf, which
 *    always writes at most `size` bytes and NUL-terminates the result.
 * 2. Explicit length pre-validation – we calculate the required length
 *    *before* writing so the function fails loudly instead of silently
 *    truncating.
 * 3. Path-traversal mitigation – the filename is rejected if it contains
 *    ".." components or a leading '/', preventing the caller from escaping
 *    the base directory.
 * 4. NULL / empty-string guard – invalid inputs are rejected early.
 * 5. Constant, clearly-named buffer size – one macro to change everywhere.
 */

#define BASE_DIR   "/var/app/data/"          /* must end with '/'      */
#define PATH_MAX_LEN  256                    /* total buffer capacity  */

/**
 * Returns 1 if `filename` is safe to append, 0 otherwise.
 *
 * Rejects:
 *  - NULL or empty strings
 *  - Absolute paths  (leading '/')
 *  - Directory traversal sequences  ("../", "..\", lone "..")
 */
static int is_safe_filename(const char *filename)
{
    if (filename == NULL || filename[0] == '\0') {
        fprintf(stderr, "build_path: filename is NULL or empty.\n");
        return 0;
    }

    /* Reject absolute paths */
    if (filename[0] == '/') {
        fprintf(stderr, "build_path: absolute paths are not allowed.\n");
        return 0;
    }

    /*
     * Reject any path component equal to "..".
     * Walk the string looking for ".." surrounded by '/' or string
     * boundaries.  This handles:  "../etc",  "foo/../../etc",  ".."
     */
    const char *p = filename;
    while (*p != '\0') {
        /* skip to next component */
        if (p[0] == '.' && p[1] == '.') {
            char after = p[2];
            if (after == '\0' || after == '/' || after == '\\') {
                fprintf(stderr, "build_path: path traversal detected.\n");
                return 0;
            }
        }
        /* advance past this component */
        while (*p != '\0' && *p != '/') {
            p++;
        }
        if (*p == '/') p++;   /* skip the separator */
    }

    return 1;   /* filename passed all checks */
}

/**
 * build_path – append `filename` to BASE_DIR and print the full path.
 *
 * @param filename  User-supplied file name (must not be NULL).
 * @return  0 on success, -1 on any error (reason printed to stderr).
 */
int build_path(const char *filename)
{
    char path[PATH_MAX_LEN];  /* stack buffer; size is a compile-time constant */

    /* ── 1. Validate the filename BEFORE touching the buffer ── */
    if (!is_safe_filename(filename)) {
        return -1;             /* reason already printed inside helper */
    }

    /* ── 2. Pre-flight length check ───────────────────────────
     *
     * Required bytes  =  strlen(BASE_DIR) + strlen(filename) + 1 (NUL)
     *
     * We compute this with size_t arithmetic to avoid signed overflow.
     * If the sum would exceed PATH_MAX_LEN we abort *before* any write.
     */
    size_t base_len = strlen(BASE_DIR);     /* known at compile time in  */
    size_t file_len = strlen(filename);     /* practice, but defensive   */

    /* Check for arithmetic overflow before the addition */
    if (file_len > (size_t)(PATH_MAX_LEN - 1) - base_len) {
        fprintf(stderr,
                "build_path: combined path length (%zu) would exceed "
                "buffer (%d).\n",
                base_len + file_len + 1, PATH_MAX_LEN);
        return -1;
    }

    /* ── 3. Safe, bounded copy via snprintf ───────────────────
     *
     * snprintf(dest, n, …) writes at most n-1 characters and always
     * appends a NUL terminator, so the buffer can never overflow.
     * We check the return value anyway: a negative return signals an
     * encoding error; a value >= PATH_MAX_LEN signals truncation (which
     * the length pre-check above already prevents, but we verify here
     * as a second layer of defence – "belt and suspenders").
     */
    int written = snprintf(path, sizeof(path), "%s%s", BASE_DIR, filename);

    if (written < 0) {
        fprintf(stderr, "build_path: snprintf encoding error.\n");
        return -1;
    }
    if ((size_t)written >= sizeof(path)) {
        /* Should be unreachable after the pre-flight check, but we keep
         * this guard because defensive programming assumes the pre-check
         * might have a latent bug. */
        fprintf(stderr, "build_path: snprintf output was truncated.\n");
        return -1;
    }

    /* ── 4. Success ───────────────────────────────────────────── */
    printf("Full path: %s\n", path);
    return 0;
}

/* ── Simple driver ───────────────────────────────────────────────────── */
int main(void)
{
    /* Expected: success */
    build_path("report.txt");
    build_path("subdir/notes.md");

    /* Expected: all rejected */
    build_path("../../etc/passwd");   /* traversal */
    build_path("/etc/shadow");        /* absolute  */
    build_path("");                   /* empty     */
    build_path(NULL);                 /* NULL      */

    /* Edge-case: filename exactly fills the buffer */
    char big[PATH_MAX_LEN];
    memset(big, 'a', sizeof(big) - 1);
    big[sizeof(big) - 1] = '\0';
    build_path(big);                  /* too long  */

    return 0;
}