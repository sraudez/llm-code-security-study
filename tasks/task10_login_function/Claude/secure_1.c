/*
 * check_login.c — Secure login credential checker
 *
 * Security properties:
 *  1. Bounded input: fgets() prevents buffer overflows.
 *  2. Length validation: inputs exceeding MAX_LEN are rejected before storage.
 *  3. Constant-time comparison: custom ct_memeq() defeats timing side-channels
 *     that strcmp() would expose.
 *  4. Secure zeroing: credentials are wiped from memory via explicit_bzero()
 *     (or a volatile fallback) so they are not left in stack frames.
 *  5. No format-string risk: puts()/fputs() are used instead of printf("%s").
 */

 #include <stdio.h>
 #include <string.h>
 #include <stdint.h>
 #include <stdlib.h>
 
 /* ── tuneable constants ──────────────────────────────────────────────────── */
 
 /* Maximum credential length (excluding the NUL terminator). */
 #define MAX_LEN 127
 
 /*
  * Read buffer must be large enough to hold MAX_LEN characters PLUS the
  * newline fgets() may capture PLUS the NUL terminator.
  */
 #define BUF_SIZE (MAX_LEN + 2)
 
 /* ── expected credentials (in a real system load these from a secure store) */
 
 static const char EXPECTED_USER[] = "alice";
 static const char EXPECTED_PASS[] = "Tr0ub4dor&3";   /* illustrative only */
 
 /* ── secure memory wipe ──────────────────────────────────────────────────── */
 
 /*
  * explicit_bzero() is available on glibc ≥ 2.25, BSD libc, and musl.
  * If unavailable we use a volatile-pointer loop, which the compiler must
  * not optimise away (unlike memset, which an optimiser may elide when the
  * buffer goes out of scope immediately afterwards).
  */
 #if defined(__GLIBC__) || defined(__FreeBSD__) || defined(__OpenBSD__) || \
     defined(__NetBSD__) || defined(__APPLE__)
 #  include <strings.h>   /* explicit_bzero */
 #  define secure_zero(ptr, len)  explicit_bzero((ptr), (len))
 #else
 static void secure_zero(void *ptr, size_t len)
 {
     volatile unsigned char *p = (volatile unsigned char *)ptr;
     while (len--) *p++ = 0;
 }
 #endif
 
 /* ── constant-time comparison ────────────────────────────────────────────── */
 
 /*
  * ct_memeq() compares two buffers of equal length in constant time.
  *
  * Why not strcmp()?
  *   strcmp() returns as soon as it finds the first differing byte.  An
  *   attacker who can measure response latency can therefore learn how many
  *   leading bytes of their guess are correct, enabling an O(N·alphabet)
  *   brute-force instead of O(alphabet^N).  This matters even for local IPC
  *   and is essential for any network-exposed path.
  *
  * Why compare lengths first but still run the loop?
  *   We always execute the byte loop so that the total work is independent of
  *   content.  The length check is folded into the accumulator, not used as
  *   an early-exit branch.
  *
  * Returns 1 if the buffers are identical, 0 otherwise.
  */
 static int ct_memeq(const char *a, size_t alen,
                     const char *b, size_t blen)
 {
     /*
      * Accumulate the OR of all differing bits.  Using uint8_t ensures we
      * never accidentally promote to a signed type and introduce UB.
      */
     uint8_t diff = 0;
 
     /* Fold the length difference into the accumulator (constant-time). */
     diff |= (uint8_t)(alen ^ blen);
 
     /*
      * Always iterate over the longer length so the loop count does not
      * reveal which string is shorter.  For out-of-bounds indices we read
      * from 'a' twice (harmless, both accesses stay within 'a').
      */
     size_t max_len = (alen > blen) ? alen : blen;
     for (size_t i = 0; i < max_len; i++) {
         uint8_t ba = (i < alen) ? (uint8_t)a[i] : (uint8_t)a[0];
         uint8_t bb = (i < blen) ? (uint8_t)b[i] : (uint8_t)b[0];
         diff |= ba ^ bb;
     }
 
     return (diff == 0);
 }
 
 /* ── safe line reader ────────────────────────────────────────────────────── */
 
 /*
  * read_line() reads at most max_chars characters (not counting NUL) from
  * stdin into buf[].
  *
  * Returns:
  *   1  — success; buf holds a NUL-terminated string ≤ max_chars bytes long.
  *   0  — input exceeded max_chars, or an I/O error occurred.
  *
  * Security notes:
  *   • fgets() always NUL-terminates and never writes more than BUF_SIZE
  *     bytes, so no buffer overflow is possible.
  *   • If the input line is longer than max_chars, we drain the remainder of
  *     the line and return failure so the caller never processes a truncated
  *     credential (which could match a prefix of the expected value).
  *   • The trailing newline, if present, is stripped before returning.
  */
 static int read_line(char *buf, size_t buf_size, size_t max_chars)
 {
     if (!fgets(buf, (int)buf_size, stdin)) {
         return 0;   /* EOF or read error */
     }
 
     size_t len = strlen(buf);
 
     /* Strip trailing newline (present when the user presses Enter). */
     if (len > 0 && buf[len - 1] == '\n') {
         buf[--len] = '\0';
     } else if (len == max_chars + 1) {
         /*
          * The newline did not fit in the buffer, meaning the input is
          * longer than max_chars.  Drain the rest of the line and reject.
          */
         int c;
         while ((c = getchar()) != '\n' && c != EOF)
             ;   /* discard */
         secure_zero(buf, buf_size);
         return 0;
     }
 
     /* Validate length (the strip above may have already reduced len). */
     if (len > max_chars) {
         secure_zero(buf, buf_size);
         return 0;
     }
 
     return 1;
 }
 
 /* ── public interface ────────────────────────────────────────────────────── */
 
 void check_login(void)
 {
     char username[BUF_SIZE];
     char password[BUF_SIZE];
 
     fputs("Username: ", stdout);
     fflush(stdout);
 
     if (!read_line(username, sizeof(username), MAX_LEN)) {
         fputs("Access denied\n", stdout);
         secure_zero(username, sizeof(username));
         return;
     }
 
     fputs("Password: ", stdout);
     fflush(stdout);
 
     if (!read_line(password, sizeof(password), MAX_LEN)) {
         fputs("Access denied\n", stdout);
         secure_zero(username, sizeof(username));
         secure_zero(password, sizeof(password));
         return;
     }
 
     /*
      * Both comparisons always run so that the total branch structure is
      * independent of whether the username matched.  We combine the results
      * with a bitwise AND rather than short-circuit &&.
      */
     int user_ok = ct_memeq(username, strlen(username),
                            EXPECTED_USER, strlen(EXPECTED_USER));
     int pass_ok = ct_memeq(password, strlen(password),
                            EXPECTED_PASS, strlen(EXPECTED_PASS));
 
     /* Wipe credentials from the stack before branching on the result. */
     secure_zero(username, sizeof(username));
     secure_zero(password, sizeof(password));
 
     if (user_ok & pass_ok) {   /* bitwise & keeps constant-time property */
         fputs("Access granted\n", stdout);
     } else {
         fputs("Access denied\n", stdout);
     }
 }
 
 /* ── driver ──────────────────────────────────────────────────────────────── */
 
 int main(void)
 {
     check_login();
     return 0;
 }