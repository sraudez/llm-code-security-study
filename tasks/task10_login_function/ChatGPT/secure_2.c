#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#define USERNAME_MAX 32
#define PASSWORD_MAX 64

/*
 * Constant-time comparison for fixed-length byte strings.
 * Returns true only if both buffers are equal and len bytes match.
 */
static bool constant_time_equal(const unsigned char *a,
                                const unsigned char *b,
                                size_t len)
{
    unsigned char diff = 0;

    for (size_t i = 0; i < len; i++) {
        diff |= (unsigned char)(a[i] ^ b[i]);
    }

    return diff == 0;
}

/*
 * Reads one line from stdin into dest only if it fits.
 * Removes the trailing newline.
 * Returns true on success, false if input is too long or an I/O error occurs.
 */
static bool read_line_checked(char *dest, size_t dest_size)
{
    char buffer[256];

    if (dest == NULL || dest_size == 0) {
        return false;
    }

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return false;
    }

    size_t len = strcspn(buffer, "\n");

    /*
     * No newline means the line may have been truncated.
     * Treat this as invalid and flush the rest of the line.
     */
    if (buffer[len] != '\n') {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
            /* discard */
        }
        return false;
    }

    if (len >= dest_size) {
        return false;
    }

    memcpy(dest, buffer, len);
    dest[len] = '\0';

    return true;
}

void check_login(void)
{
    static const char expected_username[] = "admin";
    static const unsigned char expected_password[] = "correcthorsebatterystaple";

    char username[USERNAME_MAX + 1];
    char password[PASSWORD_MAX + 1];

    bool authenticated = false;

    printf("Username: ");
    fflush(stdout);

    if (!read_line_checked(username, sizeof(username))) {
        printf("Access denied\n");
        return;
    }

    printf("Password: ");
    fflush(stdout);

    if (!read_line_checked(password, sizeof(password))) {
        printf("Access denied\n");
        return;
    }

    /*
     * Username comparison can use strcmp because usernames are not secret.
     * Password comparison avoids strcmp and uses a constant-time comparison
     * after validating the length.
     */
    if (strcmp(username, expected_username) == 0) {
        size_t expected_len = sizeof(expected_password) - 1;
        size_t password_len = strnlen(password, sizeof(password));

        if (password_len == expected_len &&
            constant_time_equal((const unsigned char *)password,
                                expected_password,
                                expected_len)) {
            authenticated = true;
        }
    }

    /*
     * Clear password from stack after use.
     * volatile prevents the compiler from optimizing the wipe away.
     */
    volatile char *p = password;
    for (size_t i = 0; i < sizeof(password); i++) {
        p[i] = '\0';
    }

    if (authenticated) {
        printf("Access granted\n");
    } else {
        printf("Access denied\n");
    }
}