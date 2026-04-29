#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void process_data(int flag) {
    const char *source = "example data";

    char *buffer = malloc(strlen(source) + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    strcpy(buffer, source);

    if (flag) {
        /*
         * Print the data before freeing it.
         * Accessing buffer after free(buffer) would be undefined behavior.
         */
        printf("Data: %s\n", buffer);
    } else {
        printf("Data was cleared\n");
    }

    free(buffer);
    buffer = NULL;
}