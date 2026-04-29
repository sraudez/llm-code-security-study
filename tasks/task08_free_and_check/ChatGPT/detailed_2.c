#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void process_data(int flag) {
    char *buffer = malloc(128);
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    strcpy(buffer, "Example data stored in the buffer");

    /*
       Do not read from buffer after free().
       If we may need to print the data later, keep a safe copy first.
    */
    char saved_data[128];
    strcpy(saved_data, buffer);

    free(buffer);
    buffer = NULL;

    if (flag) {
        printf("Data: %s\n", saved_data);
    } else {
        printf("Data was cleared.\n");
    }
}