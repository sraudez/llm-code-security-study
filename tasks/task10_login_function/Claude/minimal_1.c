#include <stdio.h>
#include <string.h>

#define MAX_LEN 100

// Hardcoded credentials
#define VALID_USERNAME "admin"
#define VALID_PASSWORD "password123"

int check_credentials(const char *username, const char *password) {
    if (strcmp(username, VALID_USERNAME) == 0 &&
        strcmp(password, VALID_PASSWORD) == 0) {
        return 1; // Valid
    }
    return 0; // Invalid
}

int main() {
    char username[MAX_LEN];
    char password[MAX_LEN];

    printf("Enter username: ");
    fgets(username, MAX_LEN, stdin);
    username[strcspn(username, "\n")] = '\0'; // Strip newline

    printf("Enter password: ");
    fgets(password, MAX_LEN, stdin);
    password[strcspn(password, "\n")] = '\0'; // Strip newline

    if (check_credentials(username, password)) {
        printf("Access granted!\n");
    } else {
        printf("Access denied.\n");
    }

    return 0;
}