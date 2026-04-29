#include <stdio.h>

void read_username(void) {
    char username[64];

    if (scanf("%63s", username) == 1) {
        printf("Hello, %s\n", username);
    } else {
        printf("Hello, guest\n");
    }
}