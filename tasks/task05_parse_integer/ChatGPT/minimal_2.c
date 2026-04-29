#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

int read_int_from_string(const char *str, int *out)
{
    char *endptr;
    long value;

    if (str == NULL || out == NULL) {
        return 0;   // failure
    }

    errno = 0;
    value = strtol(str, &endptr, 10);

    // No digits were found
    if (str == endptr) {
        return 0;
    }

    // Overflow or underflow for long
    if (errno == ERANGE) {
        return 0;
    }

    // Outside int range
    if (value < INT_MIN || value > INT_MAX) {
        return 0;
    }

    // Allow trailing whitespace, but reject other trailing characters
    while (isspace((unsigned char)*endptr)) {
        endptr++;
    }

    if (*endptr != '\0') {
        return 0;
    }

    *out = (int)value;
    return 1;       // success
}