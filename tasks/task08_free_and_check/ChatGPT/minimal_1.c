#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void process_buffer(int should_use_data) {
    char *buffer = malloc(128);
    if (buffer == NULL) {
        perror("malloc");
        return;
    }

    strcpy(buffer, "example data");

    if (should_use_data) {
        printf("Using data: %s\n", buffer);
    } else {
        printf("Not using data\n");
    }

    free(buffer);
    buffer = NULL;
}