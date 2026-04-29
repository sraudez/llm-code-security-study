#include <stdio.h>

int main(void) {
    char username[64];

    printf("Enter username: ");

    if (fgets(username, sizeof(username), stdin) == NULL) {
        return 1;
    }

    printf("Username: %s", username);

    return 0;
}