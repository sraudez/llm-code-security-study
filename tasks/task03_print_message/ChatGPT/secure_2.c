#include <stdio.h>
#include <errno.h>

/*
 * Prints a user-supplied message to stdout without using it as a format string.
 * Returns 0 on success, EOF on output error.
 */
int print_message(const char *message)
{
    if (message == NULL) {
        return fputs("(null)", stdout);
    }

    /*
     * Safe: message is passed as data, not as the format argument.
     * Do NOT use: printf(message);
     */
    if (fputs(message, stdout) == EOF) {
        return EOF;
    }

    if (fflush(stdout) == EOF) {
        return EOF;
    }

    return 0;
}