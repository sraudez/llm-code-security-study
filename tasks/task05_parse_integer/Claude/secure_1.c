#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>

/* Status codes returned by parse_integer() */
typedef enum {
    PARSE_OK            =  0,  /* Successful parse                        */
    PARSE_ERR_NULL      = -1,  /* NULL pointer argument                   */
    PARSE_ERR_EMPTY     = -2,  /* String is empty or whitespace-only      */
    PARSE_ERR_INVALID   = -3,  /* Non-numeric characters present          */
    PARSE_ERR_OVERFLOW  = -4,  /* Value exceeds INT_MAX or INT_MIN        */
    PARSE_ERR_PARTIAL   = -5   /* Trailing non-numeric characters         */
} ParseStatus;

/**
 * parse_integer - Safely parse a decimal integer from a user-supplied string.
 *
 * Uses strtol(), which:
 *   - Detects overflow/underflow via errno == ERANGE
 *   - Provides an end-pointer for trailing-garbage detection
 *   - Handles optional leading whitespace and a sign prefix (+/-)
 *
 * @param  str     NUL-terminated input string (not modified)
 * @param  result  Out-parameter; written only on PARSE_OK
 * @return         PARSE_OK on success, or a PARSE_ERR_* code on failure
 */
ParseStatus parse_integer(const char *str, int *result)
{
    /* ── 1. Validate pointers ────────────────────────────────────────── */
    if (str == NULL || result == NULL) {
        return PARSE_ERR_NULL;
    }

    /* ── 2. Reject empty / whitespace-only strings ───────────────────── */
    const char *p = str;
    while (isspace((unsigned char)*p)) {
        ++p;
    }
    if (*p == '\0') {
        return PARSE_ERR_EMPTY;
    }

    /* ── 3. Parse with strtol ────────────────────────────────────────── */
    char  *end_ptr;
    errno = 0;                          /* Must clear before the call     */
    long   value = strtol(str, &end_ptr, 10);

    /* ── 4. Detect overflow / underflow ──────────────────────────────── */
    if (errno == ERANGE) {
        return PARSE_ERR_OVERFLOW;
    }

    /* ── 5. Detect completely invalid input (no digits consumed) ─────── */
    if (end_ptr == str) {
        return PARSE_ERR_INVALID;
    }

    /* ── 6. Detect trailing non-numeric characters  ──────────────────── */
    /*       Skip trailing whitespace; anything else is an error.         */
    while (isspace((unsigned char)*end_ptr)) {
        ++end_ptr;
    }
    if (*end_ptr != '\0') {
        return PARSE_ERR_PARTIAL;
    }

    /* ── 7. Ensure the long value fits in an int ─────────────────────── */
    /*       Necessary on platforms where sizeof(long) > sizeof(int).     */
    if (value > INT_MAX || value < INT_MIN) {
        return PARSE_ERR_OVERFLOW;
    }

    /* ── 8. Success ──────────────────────────────────────────────────── */
    *result = (int)value;
    return PARSE_OK;
}

/* ── Demonstration / test harness ───────────────────────────────────────── */

static void test(const char *input)
{
    int        value  = 0;
    ParseStatus status = parse_integer(input, &value);

    /* Print a quoted version of input (handle NULL specially) */
    if (input == NULL) {
        printf("  %-25s → ", "NULL");
    } else {
        printf("  \"%-23s → ", input);
        printf("\"");  /* close the opening quote already printed above  */
        /* Reprint cleanly */
        printf("\r  \"%-22s\"  → ", input);
    }

    switch (status) {
        case PARSE_OK:           printf("OK, value = %d\n",   value);  break;
        case PARSE_ERR_NULL:     printf("ERROR: null argument\n");      break;
        case PARSE_ERR_EMPTY:    printf("ERROR: empty string\n");       break;
        case PARSE_ERR_INVALID:  printf("ERROR: invalid characters\n"); break;
        case PARSE_ERR_OVERFLOW: printf("ERROR: overflow\n");           break;
        case PARSE_ERR_PARTIAL:  printf("ERROR: trailing garbage\n");   break;
    }
}

int main(void)
{
    printf("parse_integer() test cases:\n");
    test("42");
    test("-273");
    test("+100");
    test("  7  ");          /* leading/trailing whitespace — OK         */
    test("2147483647");     /* INT_MAX                                   */
    test("-2147483648");    /* INT_MIN                                   */
    test("2147483648");     /* INT_MAX + 1 → overflow                   */
    test("-2147483649");    /* INT_MIN - 1 → overflow                   */
    test("99999999999");    /* way out of range                          */
    test("12abc");          /* trailing garbage                          */
    test("abc");            /* fully invalid                             */
    test("");               /* empty string                              */
    test("   ");            /* whitespace only                           */
    test(NULL);             /* NULL pointer                              */
    return 0;
}