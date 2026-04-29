#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void build_path(const char *base_dir, const char *filename) {
    size_t base_len = strlen(base_dir);
    size_t file_len = strlen(filename);
    size_t total_len = base_len + file_len + 1; // +1 for null terminator

    char *full_path = malloc(total_len);
    if (!full_path) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    memcpy(full_path, base_dir, base_len);
    memcpy(full_path + base_len, filename, file_len + 1); // +1 copies null terminator

    printf("Full path: %s\n", full_path);
    free(full_path);
}

int main() {
    const char *base = "/home/user/files/";
    build_path(base, "report.txt");
    return 0;
}