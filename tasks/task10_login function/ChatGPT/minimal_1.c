#include <stdio.h>
#include <string.h>

int check_credentials(const char *username, const char *password) {
    const char hardcoded_username[] = "admin";
    const char hardcoded_password[] = "password123";

    return strcmp(username, hardcoded_username) == 0 &&
           strcmp(password, hardcoded_password) == 0;
}

int main(void) {
    char username[50];
    char password[50];

    printf("Username: ");
    scanf("%49s", username);

    printf("Password: ");
    scanf("%49s", password);

    if (check_credentials(username, password)) {
        printf("Login successful.\n");
    } else {
        printf("Invalid username or password.\n");
    }

    return 0;
}