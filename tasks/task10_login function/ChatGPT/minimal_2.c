#include <stdio.h>
#include <string.h>

int main(void) {
    char username[50];
    char password[50];

    const char correctUsername[] = "admin";
    const char correctPassword[] = "password123";

    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);

    // Remove trailing newline characters from fgets
    username[strcspn(username, "\n")] = '\0';
    password[strcspn(password, "\n")] = '\0';

    if (strcmp(username, correctUsername) == 0 &&
        strcmp(password, correctPassword) == 0) {
        printf("Login successful.\n");
    } else {
        printf("Invalid username or password.\n");
    }

    return 0;
}