#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>

/* Status codes */
typedef enum {
    PARSE_OK            =  0,  /* Success                              */
    PARSE_ERR_NULL      = -1,  /* NULL pointer passed                  */
    PARSE_ERR_EMPTY     = -2,  /* Empty or whitespace-only string      */
    PARSE_ERR_INVALID   = -3,  /* Non-numeric characters in input      */
    PARSE_ERR_OVERFLOW  = -4,  /* Value exceeds INT_MAX                */
    PARSE_ERR_UNDERFLOW = -5   /* Value is below INT_MIN               */
} ParseStatus;

/*
 * parse_integer - Safely parse a decimal integer from a string.
 *
 * Parameters:
 *   str    - Null-terminated input string (user-supplied).
 *   result - Out-parameter; written only on PARSE_OK.
 *
 * Returns:
 *   PARSE_OK on success; a negative ParseStatus code on failure.
 *
 * Security properties:
 *   - Rejects NULL input.
 *   - Rejects empty / whitespace-only strings.
 *   - Rejects strings with trailing garbage (e.g. "42abc").
 *   - Detects LONG_MIN / LONG_MAX overflow via errno.
 *   - Rejects long values outside [INT_MIN, INT_MAX].
 *   - Never writes to *result unless parsing fully succeeds.
 */
ParseStatus parse_integer(const char *str, int *result)
{
    /* ── 1. Validate pointers ─────────────────────────────────────── */
    if (str == NULL || result == NULL) {
        return PARSE_ERR_NULL;
    }

    /* ── 2. Reject empty / whitespace-only strings ────────────────── */
    const char *p = str;
    while (isspace((unsigned char)*p)) {
        ++p;
    }
    if (*p == '\0') {
        return PARSE_ERR_EMPTY;
    }

    /* ── 3. Parse with strtol ─────────────────────────────────────── */
    char  *endptr = NULL;
    errno = 0;                          /* Must clear before the call  */
    long   value  = strtol(str, &endptr, 10);

    /* ── 4. Check for overflow / underflow at the long level ──────── */
    if (errno == ERANGE) {
        return (value == LONG_MAX) ? PARSE_ERR_OVERFLOW : PARSE_ERR_UNDERFLOW;
    }

    /* ── 5. Reject strings where strtol consumed nothing ─────────── */
    /*      (happens when the very first non-space char is non-digit)  */
    if (endptr == str) {
        return PARSE_ERR_INVALID;
    }

    /* ── 6. Reject trailing non-whitespace garbage ────────────────── */
    while (isspace((unsigned char)*endptr)) {
        ++endptr;
    }
    if (*endptr != '\0') {
        return PARSE_ERR_INVALID;
    }

    /* ── 7. Narrowing check: long → int range ─────────────────────── */
    if (value > INT_MAX) {
        return PARSE_ERR_OVERFLOW;
    }
    if (value < INT_MIN) {
        return PARSE_ERR_UNDERFLOW;
    }

    /* ── 8. All checks passed — write output and return success ────── */
    *result = (int)value;
    return PARSE_OK;
}

/* ── Demonstration ──────────────────────────────────────────────────── */
static void demo(const char *input)
{
    int value = 0;
    ParseStatus status = parse_integer(input, &value);

    /* Print a quoted version of the input (handle NULL display) */
    if (input == NULL) {
        printf("  Input: NULL\n");
    } else {
        printf("  Input: \"%s\"\n", input);
    }

    switch (status) {
        case PARSE_OK:
            printf("  Result: %d\n\n", value);
            break;
        case PARSE_ERR_NULL:
            printf("  Error: NULL argument\n\n");
            break;
        case PARSE_ERR_EMPTY:
            printf("  Error: empty or whitespace-only string\n\n");
            break;
        case PARSE_ERR_INVALID:
            printf("  Error: invalid characters in input\n\n");
            break;
        case PARSE_ERR_OVERFLOW:
            printf("  Error: value exceeds INT_MAX (%d)\n\n", INT_MAX);
            break;
        case PARSE_ERR_UNDERFLOW:
            printf("  Error: value below INT_MIN (%d)\n\n", INT_MIN);
            break;
    }
}

int main(void)
{
    printf("=== parse_integer test cases ===\n\n");
    demo("42");
    demo("-7");
    demo("  21  ");       /* leading/trailing whitespace */
    demo("2147483647");   /* INT_MAX */
    demo("-2147483648");  /* INT_MIN */
    demo("2147483648");   /* INT_MAX + 1 → overflow */
    demo("-2147483649");  /* INT_MIN - 1 → underflow */
    demo("99999999999");  /* >> long range on most platforms */
    demo("12abc");        /* trailing garbage */
    demo("abc");          /* no digits at all */
    demo("");             /* empty string */
    demo("   ");          /* whitespace only */
    demo(NULL);           /* NULL pointer */
    return 0;
}