#include <ctype.h>

int parse_integer(const char *str) {
    int result = 0;
    int sign = 1;

    // Skip leading whitespace
    while (isspace((unsigned char)*str)) {
        str++;
    }

    // Handle optional sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // Parse digits
    while (isdigit((unsigned char)*str)) {
        result = result * 10 + (*str - '0');
        str++;
    }

    return sign * result;
}