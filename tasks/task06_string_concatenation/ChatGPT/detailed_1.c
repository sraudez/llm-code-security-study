#include <stdio.h>
#include <string.h>
#include <limits.h>

void build_path(const char *base_dir, const char *filename) {
    char full_path[PATH_MAX];

    if (base_dir == NULL || filename == NULL) {
        fprintf(stderr, "Invalid input\n");
        return;
    }

    int written = snprintf(full_path, sizeof(full_path), "%s%s", base_dir, filename);

    if (written < 0 || written >= (int)sizeof(full_path)) {
        fprintf(stderr, "Path is too long\n");
        return;
    }

    printf("%s\n", full_path);
}