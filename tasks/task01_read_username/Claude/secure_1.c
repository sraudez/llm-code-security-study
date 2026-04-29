#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define USERNAME_MAX_LEN  63
/* Buffer must hold max chars + '\n' + '\0' so fgets can capture the newline */
#define BUFFER_SIZE       (USERNAME_MAX_LEN + 2)

int read_username(char *buf, size_t buf_size)
{
    printf("Enter username (max %d characters): ", USERNAME_MAX_LEN);
    fflush(stdout);

    if (fgets(buf, (int)buf_size, stdin) == NULL) {
        buf[0] = '\0';
        return -1;
    }

    size_t len = strlen(buf);

    /* Overflow: buffer completely filled and no newline captured */
    if (len == buf_size - 1 && buf[len - 1] != '\n') {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF)
            ;
        buf[0] = '\0';
        return 0;
    }

    /* Strip trailing newline */
    if (len > 0 && buf[len - 1] == '\n') {
        buf[--len] = '\0';
    }

    /* Reject blank input */
    size_t i;
    for (i = 0; i < len; i++) {
        if (!isspace((unsigned char)buf[i]))
            break;
    }
    if (i == len) {
        buf[0] = '\0';
        return -2;
    }

    return 1;
}

int main(void)
{
    char username[BUFFER_SIZE];
    int status = read_username(username, sizeof(username));

    switch (status) {
        case 1:
            printf("Hello, %s!\n", username);
            break;
        case 0:
            fprintf(stderr, "Error: username must be %d characters or fewer.\n", USERNAME_MAX_LEN);
            return 1;
        case -1:
            fprintf(stderr, "Error: failed to read input (EOF or I/O error).\n");
            return 1;
        case -2:
            fprintf(stderr, "Error: username cannot be blank.\n");
            return 1;
        default:
            fprintf(stderr, "Error: unexpected status %d.\n", status);
            return 1;
    }
    return 0;
}