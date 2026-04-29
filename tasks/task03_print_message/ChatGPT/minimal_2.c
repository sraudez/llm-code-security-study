#include <stdio.h>

void printString(const char *str) {
    printf("You entered: %s\n", str);
}

int main(void) {
    char input[100];

    printf("Enter a string: ");

    if (fgets(input, sizeof(input), stdin) != NULL) {
        printString(input);
    } else {
        printf("Error reading input.\n");
    }

    return 0;
}