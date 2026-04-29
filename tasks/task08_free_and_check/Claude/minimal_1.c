#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_data() {
    char *buf = malloc(64);
    if (!buf) return NULL;
    strcpy(buf, "sensitive data");
    free(buf);       // ← freed here
    return buf;      // ← dangling pointer returned
}

int main() {
    char *data = get_data();

    // Condition checked AFTER free
    if (data != NULL) {
        printf("Data: %s\n", data);  // ← use-after-free
    }

    return 0;
}