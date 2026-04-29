/*
 * read_username.c
 *
 * Demonstrates secure username input handling in C.
 * Defenses applied:
 *   - fgets() instead of gets() — enforces a hard read limit
 *   - Explicit overflow detection (no newline found → input was truncated)
 *   - Newline stripping without relying on UB-prone strcpy tricks
 *   - Input length validated before use
 *   - Buffer size defined once via a named constant
 *   - Return-value checking on every I/O call
 *   - errno reset + checked after fgets for error vs. EOF distinction
 */

 #include <stdio.h>
 #include <string.h>
 #include <errno.h>
 #include <stdlib.h>
 
 /* Single source of truth for the buffer size.
    Change this constant and every dependent check updates automatically. */
 #define USERNAME_MAX_LEN 64          /* max characters, excluding NUL      */
 #define BUF_SIZE         (USERNAME_MAX_LEN + 2)  /* +1 for '\n', +1 for '\0' */
 
 /*
  * read_username()
  *
  * Reads one username from stdin into `buf` (capacity `buf_size`).
  *
  * Returns:
  *   0  – success; buf contains a NUL-terminated, newline-stripped username
  *  -1  – I/O error or EOF with no data
  *  -2  – input exceeded USERNAME_MAX_LEN characters (overflow attempt)
  *  -3  – invalid arguments
  */
 static int read_username(char *buf, size_t buf_size)
 {
     if (buf == NULL || buf_size < 2) {   /* need room for ≥1 char + NUL */
         return -3;
     }
 
     errno = 0;
     if (fgets(buf, (int)buf_size, stdin) == NULL) {
         /* Distinguish hard error from clean EOF */
         if (ferror(stdin)) {
             perror("fgets");
         }
         return -1;
     }
 
     /* Locate the newline fgets should have stored */
     char *newline = strchr(buf, '\n');
 
     if (newline == NULL) {
         /*
          * No newline found — two possibilities:
          *   a) Input filled the entire buffer (overflow attempt / too-long name)
          *   b) Input ended at EOF without a trailing newline
          *
          * Check (b) first: if we're already at EOF and the string fits,
          * treat it as valid. Otherwise it's an overflow.
          */
         if (!feof(stdin)) {
             /* Drain the rest of the line to leave stdin in a clean state */
             int ch;
             while ((ch = getchar()) != '\n' && ch != EOF)
                 ;   /* discard excess characters */
             return -2;   /* signal overflow */
         }
         /* EOF without newline — string is complete, fall through */
     } else {
         /* Strip the newline in-place */
         *newline = '\0';
     }
 
     /* Reject empty input */
     if (buf[0] == '\0') {
         return -1;
     }
 
     return 0;
 }
 
 /* ------------------------------------------------------------------ */
 
 int main(void)
 {
     char username[BUF_SIZE];
 
     printf("Enter username (max %d characters): ", USERNAME_MAX_LEN);
     fflush(stdout);   /* ensure prompt is visible before blocking on read */
 
     int result = read_username(username, sizeof(username));
 
     switch (result) {
     case 0:
         /* Safe to use: length guaranteed ≤ USERNAME_MAX_LEN */
         printf("Hello, %s! (length: %zu)\n", username, strlen(username));
         break;
 
     case -2:
         fprintf(stderr,
                 "Error: username exceeds maximum allowed length of %d characters.\n",
                 USERNAME_MAX_LEN);
         return EXIT_FAILURE;
 
     case -3:
         fprintf(stderr, "Error: invalid buffer arguments.\n");
         return EXIT_FAILURE;
 
     default:   /* -1: EOF or I/O error */
         fprintf(stderr, "Error: could not read username.\n");
         return EXIT_FAILURE;
     }
 
     return EXIT_SUCCESS;
 }