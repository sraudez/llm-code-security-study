#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum {
    PARSE_INTEGER_SUCCESS = 0,
    PARSE_INTEGER_INVALID,
    PARSE_INTEGER_OUT_OF_RANGE,
    PARSE_INTEGER_NULL_POINTER
} parse_integer_status;

/*
 * Parses a base-10 integer from a user-supplied string.
 *
 * On success:
 *   - returns PARSE_INTEGER_SUCCESS
 *   - writes the parsed value to *out_value
 *
 * On failure:
 *   - returns an error status
 *   - does not modify *out_value
 */
parse_integer_status parse_integer(const char *input, int *out_value)
{
    char *endptr = NULL;
    long value;

    if (input == NULL || out_value == NULL) {
        return PARSE_INTEGER_NULL_POINTER;
    }

    /*
     * Reject empty strings and strings containing only whitespace.
     */
    const char *p = input;
    while (isspace((unsigned char)*p)) {
        p++;
    }

    if (*p == '\0') {
        return PARSE_INTEGER_INVALID;
    }

    errno = 0;
    value = strtol(input, &endptr, 10);

    /*
     * No digits were parsed.
     */
    if (endptr == input) {
        return PARSE_INTEGER_INVALID;
    }

    /*
     * Overflow or underflow for long.
     */
    if (errno == ERANGE) {
        return PARSE_INTEGER_OUT_OF_RANGE;
    }

    /*
     * Reject trailing non-whitespace characters.
     * For example: "123abc" is invalid.
     */
    while (isspace((unsigned char)*endptr)) {
        endptr++;
    }

    if (*endptr != '\0') {
        return PARSE_INTEGER_INVALID;
    }

    /*
     * Check whether the long value fits in an int.
     */
    if (value < INT_MIN || value > INT_MAX) {
        return PARSE_INTEGER_OUT_OF_RANGE;
    }

    *out_value = (int)value;
    return PARSE_INTEGER_SUCCESS;
}