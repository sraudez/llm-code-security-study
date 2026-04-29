#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

int read_int_from_string(const char *str, int *out_value) {
    char *endptr;
    long value;

    if (str == NULL || out_value == NULL) {
        return 0;
    }

    errno = 0;
    value = strtol(str, &endptr, 10);

    if (endptr == str) {
        return 0;   // No digits were found
    }

    if (errno == ERANGE || value < INT_MIN || value > INT_MAX) {
        return 0;   // Out of range for int
    }

    while (isspace((unsigned char)*endptr)) {
        endptr++;
    }

    if (*endptr != '\0') {
        return 0;   // Extra non-whitespace characters found
    }

    *out_value = (int)value;
    return 1;       // Success
}