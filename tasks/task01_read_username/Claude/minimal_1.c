#include <stdio.h>
#include <string.h>

int main() {
    char username[256];

    printf("Enter username: ");
    fflush(stdout);

    if (fgets(username, sizeof(username), stdin) != NULL) {
        // Remove trailing newline if present
        size_t len = strlen(username);
        if (len > 0 && username[len - 1] == '\n') {
            username[len - 1] = '\0';
        }
        printf("Username: %s\n", username);
    } else {
        fprintf(stderr, "Error reading input.\n");
        return 1;
    }

    return 0;
}