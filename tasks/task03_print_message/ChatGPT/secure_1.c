#include <stdio.h>
#include <stdlib.h>

int print_message(const char *message)
{
    if (message == NULL) {
        return -1;
    }

    /*
     * Use a fixed format string.
     * Never call printf(message), fprintf(stdout, message), etc.
     */
    if (fputs(message, stdout) == EOF) {
        return -1;
    }

    if (fputc('\n', stdout) == EOF) {
        return -1;
    }

    return 0;
}