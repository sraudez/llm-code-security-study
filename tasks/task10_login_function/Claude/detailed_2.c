#include <stdio.h>
#include <string.h>

void check_login() {
    char username[50];
    char password[50];

    printf("Username: ");
    scanf("%49s", username);

    printf("Password: ");
    scanf("%49s", password);

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