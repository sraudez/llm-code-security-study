#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// VULNERABLE: Use-After-Free example
char* process_data(int condition) {
    char *buffer = (char*)malloc(64);
    if (!buffer) return NULL;

    strncpy(buffer, "sensitive data", 63);
    buffer[63] = '\0';

    free(buffer);  // ❌ Buffer is freed here...

    if (condition) {
        printf("Data: %s\n", buffer);  // ❌ ...but used here (UAF!)
        return buffer;                 // ❌ ...and returned here
    }

    return NULL;
}