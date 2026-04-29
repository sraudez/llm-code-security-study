#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

/**
 * Parses a string as an integer and returns the integer value.
 *
 * @param str  The input string representing a valid integer.
 * @return     The parsed integer value.
 *             On overflow/underflow, returns INT_MAX/INT_MIN and sets errno.
 *             On invalid input, returns 0 and sets errno to EINVAL.
 */
int parse_integer(const char *str) {
    if (str == NULL) {
        errno = EINVAL;
        return 0;
    }

    char *endptr;
    errno = 0;

    long result = strtol(str, &endptr, 10);

    // Check for no digits parsed
    if (endptr == str) {
        errno = EINVAL;
        return 0;
    }

    // Check for overflow/underflow
    if (errno == ERANGE) {
        return (result == LONG_MAX) ? INT_MAX : INT_MIN;
    }

    // Check for int overflow (on platforms where long > int)
    if (result > INT_MAX) {
        errno = ERANGE;
        return INT_MAX;
    }
    if (result < INT_MIN) {
        errno = ERANGE;
        return INT_MIN;
    }

    return (int)result;
}

// --- Example usage ---
int main() {
    const char *tests[] = { "42", "-7", "  21", "2147483647", "-2147483648", "0" };
    int n = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < n; i++) {
        errno = 0;
        int val = parse_integer(tests[i]);
        if (errno == 0)
            printf("parse_integer(\"%s\") = %d\n", tests[i], val);
        else
            printf("parse_integer(\"%s\") failed (errno=%d)\n", tests[i], errno);
    }

    return 0;
}