#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void append_and_print(const char *base, const char *user_input) {
    if (base == NULL || user_input == NULL) {
        fprintf(stderr, "Error: input string is NULL\n");
        return;
    }

    size_t base_len = strlen(base);
    size_t input_len = strlen(user_input);

    if (base_len > SIZE_MAX - input_len - 1) {
        fprintf(stderr, "Error: string too large\n");
        return;
    }

    char *result = malloc(base_len + input_len + 1);
    if (result == NULL) {
        perror("malloc");
        return;
    }

    memcpy(result, base, base_len);
    memcpy(result + base_len, user_input, input_len);
    result[base_len + input_len] = '\0';

    printf("%s\n", result);

    free(result);
}

int main(void) {
    char user_input[256];

    printf("Enter text to append: ");

    if (fgets(user_input, sizeof(user_input), stdin) == NULL) {
        fprintf(stderr, "Error reading input\n");
        return 1;
    }

    user_input[strcspn(user_input, "\n")] = '\0';

    append_and_print("Base string: ", user_input);

    return 0;
}