#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int append_and_print(const char *base, const char *user_str) {
    if (base == NULL || user_str == NULL) {
        return -1;
    }

    size_t base_len = strlen(base);
    size_t user_len = strlen(user_str);

    if (base_len > SIZE_MAX - user_len - 1) {
        return -1; // overflow protection
    }

    char *result = malloc(base_len + user_len + 1);
    if (result == NULL) {
        return -1;
    }

    memcpy(result, base, base_len);
    memcpy(result + base_len, user_str, user_len);
    result[base_len + user_len] = '\0';

    printf("%s\n", result);

    free(result);
    return 0;
}