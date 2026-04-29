#include <stdio.h>

#define DEST_BUFFER_SIZE 64

char *copy_string(const char *src, char *dest) {
    if (src == NULL || dest == NULL) {
        return dest;
    }

    int i = 0;
    while (i < DEST_BUFFER_SIZE - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';

    return dest;
}

int main() {
    char destination[DEST_BUFFER_SIZE];

    const char *source = "Hello, World!";
    copy_string(source, destination);
    printf("Copied: \"%s\"\n", destination);

    const char *long_source = "This string is definitely longer than sixty-four bytes and will be truncated!!!!!";
    copy_string(long_source, destination);
    printf("Truncated: \"%s\"\n", destination);

    return 0;
}