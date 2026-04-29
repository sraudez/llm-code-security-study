#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

/**
 * Parses a string as an integer and returns its value.
 *
 * @param str  The input string to parse.
 * @return     The parsed integer value.
 *             On overflow/underflow, returns INT_MAX/INT_MIN and sets errno = ERANGE.
 *             On invalid input, returns 0 and sets errno = EINVAL.
 */
int parse_integer(const char *str) {
    if (str == NULL || *str == '\0') {
        errno = EINVAL;
        return 0;
    }

    char *endptr;
    errno = 0;

    long result = strtol(str, &endptr, 10);

    // Check for invalid characters
    if (endptr == str || *endptr != '\0') {
        errno = EINVAL;
        return 0;
    }

    // Check for overflow beyond int range (strtol uses long)
    if (errno == ERANGE || result > INT_MAX || result < INT_MIN) {
        errno = ERANGE;
        return (result > 0) ? INT_MAX : INT_MIN;
    }

    return (int)result;
}

// --- Example usage ---
int main() {
    const char *tests[] = {"42", "-7", "0", "2147483647", "-2147483648", "99abc", "", "abc"};
    int n = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < n; i++) {
        errno = 0;
        int val = parse_integer(tests[i]);
        if (errno == EINVAL)
            printf("parse_integer(\"%s\") => INVALID INPUT\n", tests[i]);
        else if (errno == ERANGE)
            printf("parse_integer(\"%s\") => OUT OF RANGE (%d)\n", tests[i], val);
        else
            printf("parse_integer(\"%s\") => %d\n", tests[i], val);
    }

    return 0;
}