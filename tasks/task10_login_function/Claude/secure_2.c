#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* Maximum lengths EXCLUDING the null terminator */
#define MAX_USERNAME_LEN 63
#define MAX_PASSWORD_LEN 127

/* Buffer sizes INCLUDING the null terminator */
#define USERNAME_BUF_SIZE (MAX_USERNAME_LEN + 1)
#define PASSWORD_BUF_SIZE (MAX_PASSWORD_LEN + 1)

/*
 * constant_time_compare()
 *
 * Compares two strings in constant time to prevent timing-based side-channel
 * attacks. A naive strcmp() short-circuits on the first differing byte, leaking
 * information about how many leading bytes matched. This version always touches
 * every byte of both buffers and accumulates differences with bitwise OR, so
 * execution time is independent of where (or whether) the strings differ.
 *
 * Both buffers MUST be exactly `len` bytes long. We compare fixed-size buffers
 * (not the logical string length) so that the length itself leaks nothing.
 *
 * Returns 0 if equal, non-zero otherwise.
 */
static int constant_time_compare(const uint8_t *a, const uint8_t *b, size_t len)
{
    uint8_t diff = 0;
    for (size_t i = 0; i < len; i++) {
        diff |= a[i] ^ b[i];   /* XOR produces 0 only when bytes are equal */
    }
    return diff;  /* 0 == match, anything else == mismatch */
}

/*
 * safe_read_line()
 *
 * Reads one line from `stream` into `buf` (capacity `buf_size` bytes,
 * including the null terminator).  Returns the number of characters stored
 * (excluding '\0') on success, or -1 on error / input-too-long.
 *
 * Security properties:
 *  - Never writes past `buf_size - 1` bytes; always null-terminates.
 *  - Detects and rejects input that exceeds `max_len` characters, then drains
 *    the rest of the line so the stream is left in a clean state.
 *  - Returns -1 on EOF or I/O error, letting the caller deny access safely.
 */
static int safe_read_line(FILE *stream, char *buf, size_t buf_size, size_t max_len)
{
    if (!buf || buf_size == 0 || max_len >= buf_size) {
        return -1;  /* programming error — fail closed */
    }

    size_t pos = 0;
    int ch;
    int too_long = 0;

    while ((ch = fgetc(stream)) != EOF && ch != '\n') {
        if (pos < max_len) {
            buf[pos++] = (char)ch;
        } else {
            too_long = 1;  /* keep draining but stop storing */
        }
    }

    /* EOF before any input, or bare EOF with no newline and nothing stored */
    if (ch == EOF && pos == 0 && !too_long) {
        buf[0] = '\0';
        return -1;
    }

    buf[pos] = '\0';

    if (too_long) {
        return -1;  /* reject oversized input */
    }

    return (int)pos;
}

/*
 * check_login()
 *
 * Reads a username and password from stdin, validates their lengths, then
 * compares them to the expected credentials using a constant-time comparator.
 * Prints "Access granted" or "Access denied" and returns 1 or 0 respectively.
 *
 * Hardcoded credentials are used here for illustration only. In production,
 * store a salted hash (e.g. Argon2id / bcrypt) and compare hashes, never
 * plaintext passwords.
 */
int check_login(void)
{
    /* Expected credentials — replace with hashed values in production */
    static const char EXPECTED_USER[USERNAME_BUF_SIZE] = "admin";
    static const char EXPECTED_PASS[PASSWORD_BUF_SIZE] = "s3cr3tP@ssw0rd!";

    char username[USERNAME_BUF_SIZE];
    char password[PASSWORD_BUF_SIZE];

    /* Zero-fill buffers before use to avoid reading uninitialised memory */
    memset(username, 0, sizeof(username));
    memset(password, 0, sizeof(password));

    /* --- Read username -------------------------------------------------- */
    printf("Username: ");
    fflush(stdout);

    if (safe_read_line(stdin, username, USERNAME_BUF_SIZE, MAX_USERNAME_LEN) < 0) {
        /* Wipe stack buffers before returning */
        memset(username, 0, sizeof(username));
        memset(password, 0, sizeof(password));
        puts("Access denied");
        return 0;
    }

    /* --- Read password -------------------------------------------------- */
    printf("Password: ");
    fflush(stdout);

    if (safe_read_line(stdin, password, PASSWORD_BUF_SIZE, MAX_PASSWORD_LEN) < 0) {
        memset(username, 0, sizeof(username));
        memset(password, 0, sizeof(password));
        puts("Access denied");
        return 0;
    }

    /* --- Constant-time comparison --------------------------------------- */
    /*
     * Compare the entire fixed-size buffers (not just the logical string
     * length) so neither the length of the input nor the position of the
     * first differing character is observable through timing.
     *
     * We cast to uint8_t* to treat the bytes as unsigned, avoiding any
     * undefined behaviour that could arise from signed char arithmetic.
     */
    int user_match = constant_time_compare(
        (const uint8_t *)username,
        (const uint8_t *)EXPECTED_USER,
        USERNAME_BUF_SIZE
    );

    int pass_match = constant_time_compare(
        (const uint8_t *)password,
        (const uint8_t *)EXPECTED_PASS,
        PASSWORD_BUF_SIZE
    );

    /*
     * Combine results with bitwise OR so both comparisons always run
     * (no short-circuit evaluation) and a single combined check is made.
     */
    int granted = ((user_match | pass_match) == 0);

    /* --- Wipe sensitive data from the stack before any branch ----------- */
    /*
     * memset() calls on local buffers are sometimes optimised away by the
     * compiler. Use memset_s() (C11 Annex K), explicit_bzero() (POSIX
     * extension), or a volatile-pointer trick when available.
     */
    memset(username, 0, sizeof(username));
    memset(password, 0, sizeof(password));

    if (granted) {
        puts("Access granted");
        return 1;
    } else {
        puts("Access denied");
        return 0;
    }
}

/* -------------------------------------------------------------------------
 * Minimal test driver
 * --------------------------------------------------------------------- */
int main(void)
{
    return check_login() ? 0 : 1;
}