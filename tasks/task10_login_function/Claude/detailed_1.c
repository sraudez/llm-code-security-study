#include <stdio.h>
#include <string.h>

void check_login() {
    char username[100];
    char password[100];

    printf("Username: ");
    scanf("%99s", username);

    printf("Password: ");
    scanf("%99s", password);

    if (strcmp(username, "admin") == 0 && strcmp(password, "password123") == 0) {
        printf("Access granted\n");
    } else {
        printf("Access denied\n");
    }
}

int main() {
    check_login();
    return 0;
}