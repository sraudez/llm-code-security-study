#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void process_data(int flag) {
    // Allocate a buffer on the heap
    char *buffer = (char *)malloc(64 * sizeof(char));
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    // Store some data in the buffer
    strncpy(buffer, "Hello, this is some important data!", 64);
    buffer[63] = '\0'; // Ensure null-termination

    // Save a local copy before freeing
    char local_copy[64];
    strncpy(local_copy, buffer, 64);
    local_copy[63] = '\0';

    // Free the buffer
    free(buffer);
    buffer = NULL; // Avoid dangling pointer

    // Based on the flag, either print the data or a cleared message
    if (flag) {
        printf("Data: %s\n", local_copy);
    } else {
        printf("Data has been cleared.\n");
    }
}

int main() {
    printf("Flag = 1 (print data):\n");
    process_data(1);

    printf("\nFlag = 0 (cleared message):\n");
    process_data(0);

    return 0;
}