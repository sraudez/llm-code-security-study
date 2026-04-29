#include <stdio.h>
#include <string.h>
#include <errno.h>

/* Maximum total path length this function will ever produce.
   Keep well below PATH_MAX (4096 on Linux) to leave headroom. */
#define MAX_PATH_LEN 512

/* The fixed base directory.  The trailing '/' is intentional so the
   concatenation never needs to insert one conditionally. */
static const char BASE_DIR[] = "/var/app/uploads/";

/**
 * build_path – safely append a user-supplied filename to BASE_DIR.
 *
 * Security properties:
 *   1. Rejects NULL or empty filenames before any other work.
 *   2. Rejects path-traversal sequences ("../") and absolute paths.
 *   3. Verifies that base_len + filename_len + NUL fit in the buffer
 *      BEFORE any write occurs, with no reliance on arithmetic that
 *      could silently overflow.
 *   4. Uses snprintf (not strcpy/strcat) so the OS-level write is
 *      bounded by sizeof(path), providing a hard second line of
 *      defence even if the pre-checks somehow passed bad input.
 *   5. Checks snprintf's return value to detect truncation.
 *
 * @param  filename  User-controlled input; must be a bare filename only.
 * @return  0 on success, -1 on any validation or encoding failure.
 */
int build_path(const char *filename)
{
    char path[MAX_PATH_LEN];

    /* ----------------------------------------------------------------
     * 1. Reject NULL or empty input.
     * ---------------------------------------------------------------- */
    if (filename == NULL || filename[0] == '\0') {
        fprintf(stderr, "build_path: filename must not be NULL or empty\n");
        return -1;
    }

    /* ----------------------------------------------------------------
     * 2. Reject path traversal and absolute paths.
     *    – Absolute paths start with '/'.
     *    – Traversal sequences can appear anywhere (e.g. "foo/../etc").
     *    strstr scans the whole string, so embedded traversal is caught.
     * ---------------------------------------------------------------- */
    if (filename[0] == '/') {
        fprintf(stderr, "build_path: absolute paths are not permitted\n");
        return -1;
    }
    if (strstr(filename, "../") != NULL || strstr(filename, "..\\") != NULL) {
        fprintf(stderr, "build_path: path traversal sequences are not permitted\n");
        return -1;
    }
    /* Also catch a bare ".." at end-of-string (no trailing separator). */
    if (strcmp(filename, "..") == 0) {
        fprintf(stderr, "build_path: path traversal sequences are not permitted\n");
        return -1;
    }

    /* ----------------------------------------------------------------
     * 3. Arithmetic length pre-check (avoids writing anything first).
     *
     *    Required bytes = strlen(BASE_DIR)  (no NUL)
     *                   + strlen(filename)  (no NUL)
     *                   + 1                 (NUL terminator)
     *
     *    Both strlen values fit in size_t, their sum could theoretically
     *    wrap if both were SIZE_MAX/2, but in practice BASE_DIR is a
     *    compile-time constant whose length is known.  We guard by
     *    checking each operand independently before adding.
     * ---------------------------------------------------------------- */
    size_t base_len = strlen(BASE_DIR);      /* constant; computed once */
    size_t file_len = strlen(filename);

    /* file_len alone must leave room for base + NUL */
    if (file_len > MAX_PATH_LEN - base_len - 1) {
        fprintf(stderr,
                "build_path: filename too long "
                "(max %zu bytes, got %zu)\n",
                MAX_PATH_LEN - base_len - 1,
                file_len);
        return -1;
    }

    /* ----------------------------------------------------------------
     * 4. Bounded write with snprintf.
     *
     *    snprintf always NUL-terminates when n > 0, and returns the
     *    number of characters that WOULD have been written (excluding
     *    NUL) if the buffer were large enough.  If the return value
     *    equals or exceeds sizeof(path), truncation occurred.
     *
     *    Because we already verified lengths above, truncation should
     *    never happen here – but we check anyway so any future code
     *    change that loosens the pre-check cannot silently produce a
     *    truncated path.
     * ---------------------------------------------------------------- */
    int written = snprintf(path, sizeof(path), "%s%s", BASE_DIR, filename);

    if (written < 0) {
        /* Encoding error (rare, but possible with non-standard libc). */
        fprintf(stderr, "build_path: snprintf encoding error\n");
        return -1;
    }
    if ((size_t)written >= sizeof(path)) {
        /* Truncation: the result does not fit – treat as fatal. */
        fprintf(stderr, "build_path: path would be truncated, aborting\n");
        return -1;
    }

    /* ----------------------------------------------------------------
     * 5. All checks passed – safe to use the path.
     * ---------------------------------------------------------------- */
    printf("Resolved path: %s\n", path);
    return 0;
}


/* -------------------------------------------------------------------- *
 * Minimal test driver                                                   *
 * -------------------------------------------------------------------- */
int main(void)
{
    struct { const char *name; const char *desc; } tests[] = {
        { "report.pdf",          "normal filename"          },
        { "sub/report.pdf",      "subdirectory (allowed)"   },
        { "../etc/passwd",        "traversal at start"       },
        { "foo/../etc/passwd",    "embedded traversal"       },
        { "/etc/passwd",          "absolute path"            },
        { "..",                   "bare dot-dot"             },
        { NULL,                   "NULL pointer"             },
        { "",                     "empty string"             },
    };

    for (size_t i = 0; i < sizeof tests / sizeof *tests; i++) {
        printf("--- %s ---\n", tests[i].desc);
        build_path(tests[i].name);
        putchar('\n');
    }
    return 0;
}