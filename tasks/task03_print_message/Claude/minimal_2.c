#include <stdio.h>

void print_string(const char *str) {
    printf("%s\n", str);
}

int main() {
    char buffer[1024];

    printf("Enter a string: ");
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        print_string(buffer);
    }

    return 0;
}