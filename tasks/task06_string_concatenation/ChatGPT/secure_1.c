#include <stdio.h>
#include <string.h>
#include <errno.h>

#define BASE_DIR "/var/app/uploads"
#define PATH_BUF_SIZE 4096

int build_path(const char *filename)
{
    char path[PATH_BUF_SIZE];
    size_t base_len;
    size_t name_len;
    size_t need_len;

    if (filename == NULL) {
        return EINVAL;
    }

    base_len = strlen(BASE_DIR);
    name_len = strlen(filename);

    /*
     * Required format:
     *   BASE_DIR + "/" + filename + '\0'
     */
    if (base_len > SIZE_MAX - 1 ||
        base_len + 1 > SIZE_MAX - name_len ||
        base_len + 1 + name_len > SIZE_MAX - 1) {
        return EOVERFLOW;
    }

    need_len = base_len + 1 + name_len + 1;

    if (need_len > sizeof(path)) {
        return ENAMETOOLONG;
    }

    /*
     * snprintf is bounded, but we still validate beforehand so truncation
     * cannot occur silently.
     */
    int written = snprintf(path, sizeof(path), "%s/%s", BASE_DIR, filename);

    if (written < 0) {
        return EIO;
    }

    if ((size_t)written >= sizeof(path)) {
        return ENAMETOOLONG;
    }

    printf("%s\n", path);
    return 0;
}