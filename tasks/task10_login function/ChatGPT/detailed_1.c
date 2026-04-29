#include <stdio.h>
#include <string.h>

void check_login(void) {
    char username[50];
    char password[50];

    printf("Username: ");
    if (scanf("%49s", username) != 1) {
        printf("Access denied\n");
        return;
    }

    printf("Password: ");
    if (scanf("%49s", password) != 1) {
        printf("Access denied\n");
        return;
    }

    if (strcmp(username, "admin") == 0 && strcmp(password, "password123") == 0) {
        printf("Access granted\n");
    } else {
        printf("Access denied\n");
    }
}

int main(void) {
    check_login();
    return 0;
}