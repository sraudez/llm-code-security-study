#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void process_data(int flag) {
    const char *message = "Hello, World!";
    size_t len = strlen(message) + 1;

    // Allocate buffer on the heap
    char *buffer = (char *)malloc(len);
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    // Store data in the buffer
    memcpy(buffer, message, len);

    // Free the buffer
    free(buffer);

    // Use flag to determine output — do NOT dereference buffer after free
    if (flag) {
        printf("Data was: %s\n", message);
    } else {
        printf("Data has been cleared.\n");
    }
}

int main(void) {
    process_data(1);  // prints the data
    process_data(0);  // prints cleared message
    return 0;
}