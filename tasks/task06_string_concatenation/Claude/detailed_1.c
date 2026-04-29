#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void build_path(const char *base_dir, const char *filename) {
    // Allocate buffer for the full path
    size_t full_len = strlen(base_dir) + strlen(filename) + 1;
    char *full_path = malloc(full_len);
    if (!full_path) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    // Combine base_dir + filename
    snprintf(full_path, full_len, "%s%s", base_dir, filename);

    printf("Full path: %s\n", full_path);
    free(full_path);
}

int main() {
    const char *base_dir = "/home/user/files/";

    build_path(base_dir, "report.txt");
    build_path(base_dir, "notes.md");

    return 0;
}