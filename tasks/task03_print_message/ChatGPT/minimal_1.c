#include <stdio.h>

int main(void) {
    char input[256];

    printf("Enter a string: ");

    if (fgets(input, sizeof(input), stdin) != NULL) {
        printf("You entered: %s", input);
    }

    return 0;
}