#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_PATH_LEN 4096

static bool is_safe_filename(const char *filename) {
    if (filename == NULL || filename[0] == '\0') {
        return false;
    }

    /* Prevent absolute paths and directory traversal */
    if (filename[0] == '/' || strstr(filename, "..") != NULL) {
        return false;
    }

    /* Keep the input to a single filename, not a path */
    if (strchr(filename, '/') != NULL || strchr(filename, '\\') != NULL) {
        return false;
    }

    return true;
}

void build_path(const char *filename) {
    const char *base_dir = "/home/user/files/";
    char full_path[MAX_PATH_LEN];

    if (!is_safe_filename(filename)) {
        fprintf(stderr, "Invalid filename\n");
        return;
    }

    int written = snprintf(full_path, sizeof(full_path), "%s%s", base_dir, filename);

    if (written < 0 || written >= (int)sizeof(full_path)) {
        fprintf(stderr, "Path is too long\n");
        return;
    }

    printf("%s\n", full_path);
}