#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define USERNAME_MAX 64
#define PASSWORD_MAX 128

static void drain_stdin_line(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* discard */
    }
}

static bool read_bounded_line(const char *prompt, char *out, size_t out_size) {
    char buffer[out_size + 1]; /* one extra byte to detect overly long input */

    if (out_size < 2) {
        return false;
    }

    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return false;
    }

    size_t len = strnlen(buffer, sizeof(buffer));

    /*
     * If no newline was read and the buffer is full, input exceeded the
     * allowed length. Drain the rest of the line and reject it.
     */
    if (len > 0 && buffer[len - 1] != '\n') {
        if (len == sizeof(buffer) - 1) {
            drain_stdin_line();
            return false;
        }
    } else if (len > 0) {
        buffer[len - 1] = '\0';
        len--;
    }

    if (len >= out_size) {
        return false;
    }

    memcpy(out, buffer, len + 1);
    return true;
}

static bool constant_time_equal(const char *a, const char *b, size_t max_len) {
    size_t len_a = strnlen(a, max_len + 1);
    size_t len_b = strnlen(b, max_len + 1);

    unsigned char diff = 0;

    /*
     * Include length difference in the result, but still compare max_len bytes
     * so timing does not reveal where the password differs.
     */
    diff |= (unsigned char)(len_a ^ len_b);

    for (size_t i = 0; i < max_len; i++) {
        unsigned char ca = i < len_a ? (unsigned char)a[i] : 0;
        unsigned char cb = i < len_b ? (unsigned char)b[i] : 0;
        diff |= (unsigned char)(ca ^ cb);
    }

    return diff == 0;
}

void check_login(void) {
    static const char expected_username[] = "admin";
    static const char expected_password[] = "correcthorsebatterystaple";

    char username[USERNAME_MAX + 1];
    char password[PASSWORD_MAX + 1];

    bool ok = false;

    if (read_bounded_line("Username: ", username, sizeof(username)) &&
        read_bounded_line("Password: ", password, sizeof(password))) {

        bool username_ok =
            strnlen(username, sizeof(username)) == strlen(expected_username) &&
            memcmp(username, expected_username, strlen(expected_username)) == 0;

        bool password_ok =
            constant_time_equal(password, expected_password, PASSWORD_MAX);

        ok = username_ok && password_ok;
    }

    /*
     * Clear password from memory after use.
     * volatile prevents the compiler from optimizing this away.
     */
    volatile unsigned char *p = (volatile unsigned char *)password;
    for (size_t i = 0; i < sizeof(password); i++) {
        p[i] = 0;
    }

    if (ok) {
        puts("Access granted");
    } else {
        puts("Access denied");
    }
}