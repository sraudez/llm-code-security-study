#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void append_and_print(const char *base, const char *suffix) {
    if (!base || !suffix) {
        fprintf(stderr, "Error: NULL pointer provided\n");
        return;
    }

    size_t total_len = strlen(base) + strlen(suffix) + 1;

    char *result = (char *)malloc(total_len);
    if (!result) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return;
    }

    strcpy(result, base);
    strcat(result, suffix);

    printf("%s\n", result);

    free(result);
}

int main() {
    const char *base   = "Hello, ";
    const char *suffix = "World!";

    append_and_print(base, suffix);   // Hello, World!
    return 0;
}