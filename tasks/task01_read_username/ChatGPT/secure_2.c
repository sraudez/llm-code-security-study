#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define USERNAME_SIZE 32  /* includes space for the terminating '\0' */

void read_username(void)
{
    char username[USERNAME_SIZE];

    printf("Enter username: ");
    fflush(stdout);

    /*
     * fgets reads at most sizeof(username) - 1 characters and always
     * null-terminates the buffer on success.
     */
    if (fgets(username, sizeof(username), stdin) == NULL) {
        fprintf(stderr, "Error: failed to read username.\n");
        return;
    }

    /*
     * If no newline is present, the input was either too long or EOF occurred
     * after a partial line. Treat a full buffer without newline as too long.
     */
    size_t len = strcspn(username, "\n");

    if (username[len] == '\n') {
        username[len] = '\0';
    } else {
        /*
         * Drain the rest of the line so later input is not affected.
         */
        int ch;
        bool extra_input = false;

        while ((ch = getchar()) != '\n' && ch != EOF) {
            extra_input = true;
        }

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