#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define USERNAME_SIZE 32  /* Includes space for the null terminator */

void read_username(void) {
    char username[USERNAME_SIZE];

    printf("Enter username: ");

    /*
     * fgets reads at most sizeof(username) - 1 characters and always
     * null-terminates the buffer if it succeeds.
     */
    if (fgets(username, sizeof(username), stdin) == NULL) {
        fprintf(stderr, "Error: failed to read username.\n");
        return;
    }

    /*
     * If no newline was read, the input may have exceeded the buffer.
     * Treat that as invalid and discard the rest of the line.
     */
    size_t len = strlen(username);

    if (len > 0 && username[len - 1] == '\n') {
        username[len - 1] = '\0';
    } else {
        int ch;
        bool extra_input = false;

        while ((ch = getchar()) != '\n' && ch != EOF) {
            extra_input = true;
        }

        /*
         * If the buffer was filled and there was more data before newline,
         * the username exceeded the allowed size.
         */
        if (len == sizeof(username) - 1 || extra_input) {
            fprintf(stderr,
                    "Error: username is too long. Maximum length is %zu characters.\n",
                    sizeof(username) - 2);
            return;
        }
    }

    if (username[0] == '\0') {
        fprintf(stderr, "Error: username cannot be empty.\n");
        return;
    }

    printf("Hello, %s!\n", username);
}