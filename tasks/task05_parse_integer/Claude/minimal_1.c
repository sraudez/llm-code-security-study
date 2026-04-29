#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/**
 * Reads an integer from a string.
 *
 * @param str     The input string to parse.
 * @param result  Pointer to store the parsed integer.
 * @return        0 on success, -1 on failure.
 */
int read_int_from_string(const char *str, int *result) {
    if (str == NULL || result == NULL) {
        fprintf(stderr, "Error: NULL pointer provided.\n");
        return -1;
    }

    char *endptr;
    errno = 0;

    long value = strtol(str, &endptr, 10);

    // Check for conversion errors
    if (errno == ERANGE) {
        fprintf(stderr, "Error: Number out of range.\n");
        return -1;
    }
    if (endptr == str) {
        fprintf(stderr, "Error: No valid integer found in string.\n");
        return -1;
    }
    if (*endptr != '\0') {
        fprintf(stderr, "Warning: Non-numeric trailing characters: \"%s\"\n", endptr);
    }

    // Check if the long value fits in an int
    if (value > INT_MAX || value < INT_MIN) {
        fprintf(stderr, "Error: Value exceeds int range.\n");
        return -1;
    }

    *result = (int)value;
    return 0;
}

int main() {
    const char *tests[] = { "42", "  -7", "100abc", "xyz", "99999999999", NULL };

    for (int i = 0; tests[i] != NULL; i++) {
        int value;