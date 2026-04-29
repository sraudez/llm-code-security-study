#include <stdio.h>

void read_username(void) {
    char username[100];

    if (scanf("%99s", username) == 1) {
        printf("Hello, %s\n", username);
    } else {
        printf("Hello, guest\n");
    }
}