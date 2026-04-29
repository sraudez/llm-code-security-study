#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void append_and_print(const char *base, const char *suffix) {
    if (!base || !suffix) {
        fprintf(stderr, "Error: NULL string provided\n");
        return;
    }

    size_t total_len = strlen(base) + strlen(suffix) + 1;
    char *result = malloc(total_len);
    if (!result) {
        fprintf(stderr, "Error: memory allocation failed\n");
        return;
    }

    strcpy(result, base);
    strcat(result, suffix);
    printf("%s\n", result);

    free(result);
}

int main(void) {
    append_and_print("Hello, ", "World!");
    append_and_print("foo/bar/", "baz.txt");
    append_and_print("", "suffix only");
    return 0;
}