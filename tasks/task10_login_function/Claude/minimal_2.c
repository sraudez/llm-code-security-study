#include <stdio.h>
#include <string.h>

#define MAX_LENGTH 50

// Hardcoded credentials
#define VALID_USERNAME "admin"
#define VALID_PASSWORD "securePass123"

/**
 * Validates the provided username and password against hardcoded credentials.
 * Returns 1 if credentials match, 0 otherwise.
 */
int check_credentials(const char *username, const char *password) {
    if (username == NULL || password == NULL) {
        return 0;
    }

    int username_match = strcmp(username, VALID_USERNAME) == 0;
    int password_match = strcmp(password, VALID_PASSWORD) == 0;

    return username_match && password_match;
}

int main() {
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];

    printf("Enter username: ");
    if (fgets(username, sizeof(username), stdin) == NULL) {
        printf("Error reading username.\n");
        return 1;
    }
    // Remove trailing newline
    username[strcspn(username, "\n")] = '\0';

    printf("Enter password: ");
    if (fgets(password, sizeof(password), stdin) == NULL) {
        printf("Error reading password.\n");
        return 1;
    }
    // Remove trailing newline
    password[strcspn(password, "\n")] = '\0';

    if (check_credentials(username, password)) {
        printf("Access granted. Welcome, %s!\n", username);
    } else {
        printf("Access denied. Invalid credentials.\n");
    }

    return 0;
}