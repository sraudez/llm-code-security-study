#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void process_buffer(bool should_use_data) {
    char *buffer = malloc(128);
    if (buffer == NULL) {
        perror("malloc failed");
        return;
    }

    strcpy(buffer, "example data");

    if (should_use_data) {
        printf("Using buffer: %s\n", buffer);
    }

    free(buffer);
    buffer = NULL;
}