#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

/*
 * Status codes returned by parse_integer().
 */
typedef enum {
    PARSE_INTEGER_OK = 0,
    PARSE_INTEGER_NULL_ARGUMENT,
    PARSE_INTEGER_EMPTY,
    PARSE_INTEGER_INVALID,
    PARSE_INTEGER_OUT_OF_RANGE
} parse_integer_status;

/*
 * Safely parses an int from a user-supplied string.
 *
 * Rules:
 * - Accepts leading and trailing whitespace.
 * - Accepts optional '+' or '-' sign.
 * - Rejects empty strings.
 * - Rejects strings with extra non-whitespace characters.
 * - Detects overflow and underflow.
 * - Does not modify output unless parsing succeeds.
 */
parse_integer_status parse_integer(const char *input, int *output)
{
    char *endptr = NULL;
    long value;

    if (input == NULL || output == NULL) {
        return PARSE_INTEGER_NULL_ARGUMENT;
    }

    while (isspace((unsigned char)*input)) {
        input++;
    }

    if (*input == '\0') {
        return PARSE_INTEGER_EMPTY;
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
     * strtol detected long overflow or underflow.
     */
    if (errno == ERANGE) {
        return PARSE_INTEGER_OUT_OF_RANGE;
    }

    /*
     * The parsed long value may still exceed int range.
     */
    if (value < INT_MIN || value > INT_MAX) {
        return PARSE_INTEGER_OUT_OF_RANGE;
    }

    /*
     * Allow trailing whitespace, but reject any other trailing characters.
     */
    while (isspace((unsigned char)*endptr)) {
        endptr++;
    }

    if (*endptr != '\0') {
        return PARSE_INTEGER_INVALID;
    }

    *output = (int)value;
    return PARSE_INTEGER_OK;
}