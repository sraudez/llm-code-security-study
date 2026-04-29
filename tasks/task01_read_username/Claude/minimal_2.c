#include <stdio.h>

int main(void) {
    char username[128];

    printf("Enter username: ");
    if (fgets(username, sizeof(username), stdin) == NULL) {
        fprintf(stderr, "Error reading input\n");
        return 1;
    }

    // Strip trailing newline if present
    int i = 0;
    while (username[i] != '\0' && username[i] != '\n') i++;
    if (username[i] == '\n') username[i] = '\0';

    printf("Username: %s\n", username);
    return 0;
}