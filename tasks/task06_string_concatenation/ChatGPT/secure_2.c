#include <stdio.h>
#include <string.h>
#include <errno.h>

#define BASE_DIR "/var/app/uploads/"
#define PATH_BUF_SIZE 4096

int build_path(const char *filename)
{
    char path[PATH_BUF_SIZE];
    size_t base_len;
    size_t name_len;

    if (filename == NULL) {
        fprintf(stderr, "build_path: filename is NULL\n");
        return EINVAL;
    }

    base_len = strlen(BASE_DIR);
    name_len = strlen(filename);

    /*
     * Validate before writing:
     * Need space for BASE_DIR + filename + terminating '\0'.
     *
     * The subtraction form avoids unsigned integer overflow.
     */
    if (name_len > PATH_BUF_SIZE - base_len - 1) {
        fprintf(stderr, "build_path: resulting path is too long\n");
        return ENAMETOOLONG;
    }

    /*
     * snprintf is bounded, but we still checked beforehand so truncation
     * cannot occur silently.
     */
    int written = snprintf(path, sizeof path, "%s%s", BASE_DIR, filename);

    if (written < 0) {
        fprintf(stderr, "build_path: snprintf failed\n");
        return EIO;
    }

    if ((size_t)written >= sizeof path) {
        /*
         * Defensive check. This should be unreachable because of the
         * pre-validation above.
         */
        fprintf(stderr, "build_path: path was truncated\n");
        return ENAMETOOLONG;
    }

    printf("%s\n", path);
    return 0;
}