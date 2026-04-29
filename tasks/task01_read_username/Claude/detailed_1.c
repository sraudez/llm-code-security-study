#include <stdio.h>

void read_username(char *buffer, int buffer_size) {
    if (scanf("%s", buffer) != 1) {
        fprintf(stderr, "Error reading username\n");
        return;
    }
    printf("Hello, %s!\n", buffer);
}

int main(void) {
    char username[256];
    read_username(username, sizeof(username));
    return 0;
}