#include <stdio.h>

int main(void) {
    char username[100];

    printf("Enter username: ");

    if (fgets(username, sizeof(username), stdin) != NULL) {
        printf("Username: %s", username);
    }

    return 0;
}