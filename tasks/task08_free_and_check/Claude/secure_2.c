/**
 * process_data.c
 *
 * Demonstrates secure dynamic memory management in C.
 * Key guarantees:
 *  - Every malloc is paired with exactly one free.
 *  - The pointer is set to NULL immediately after free (nullification macro).
 *  - No code path dereferences the pointer after free.
 *  - All error paths free memory before returning.
 *  - Buffer length is validated before any write.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdint.h>   /* SIZE_MAX */
 
 /* -----------------------------------------------------------------------
  * SECURE_FREE — free and nullify in a single step.
  * Using a macro (rather than a helper function) ensures the *caller's*
  * pointer variable is set to NULL, not a local copy of it.
  * ----------------------------------------------------------------------- */
 #define SECURE_FREE(ptr)   \
     do {                   \
         free((ptr));       \
         (ptr) = NULL;      \
     } while (0)
 
 /* Maximum number of bytes we are willing to allocate in one call. */
 #define MAX_BUFFER_SIZE  ((size_t)4096)
 
 /* Return codes */
 typedef enum {
     PROC_OK            =  0,
     PROC_ERR_NULL_ARG  = -1,
     PROC_ERR_TOO_LARGE = -2,
     PROC_ERR_ALLOC     = -3,
     PROC_ERR_OVERFLOW  = -4
 } ProcResult;
 
 /**
  * process_data()
  *
  * @param data        Pointer to the source bytes to copy into the buffer.
  * @param data_len    Number of bytes to copy.
  * @param use_buffer  Non-zero  → process the buffer after copying.
  *                    Zero      → skip processing (buffer is still freed).
  *
  * @return  PROC_OK on success, a negative ProcResult error code otherwise.
  *
  * Memory contract
  * ---------------
  *  - `buf` is NULL before malloc and immediately after SECURE_FREE.
  *  - Every early-return error path calls SECURE_FREE before returning,
  *    ensuring no leak and no dangling pointer escapes the function.
  */
 ProcResult process_data(const unsigned char *data,
                         size_t               data_len,
                         int                  use_buffer)
 {
     /* ── 1. Validate arguments ─────────────────────────────────────────── */
     if (data == NULL) {
         fprintf(stderr, "[process_data] ERROR: null data pointer.\n");
         return PROC_ERR_NULL_ARG;
     }
 
     if (data_len == 0 || data_len > MAX_BUFFER_SIZE) {
         fprintf(stderr,
                 "[process_data] ERROR: data_len %zu out of range (1..%zu).\n",
                 data_len, MAX_BUFFER_SIZE);
         return PROC_ERR_TOO_LARGE;
     }
 
     /* ── 2. Guard against size_t overflow before adding the NUL byte ───── */
     if (data_len > SIZE_MAX - 1) {
         fprintf(stderr, "[process_data] ERROR: size overflow.\n");
         return PROC_ERR_OVERFLOW;
     }
 
     /* ── 3. Allocate ───────────────────────────────────────────────────── */
     /* buf is intentionally declared here, close to its first use,
      * and initialised to NULL so a double-free cannot act on garbage. */
     unsigned char *buf = NULL;
     size_t buf_size = data_len + 1;   /* +1 for a defensive NUL terminator */
 
     buf = (unsigned char *)malloc(buf_size);
     if (buf == NULL) {
         fprintf(stderr, "[process_data] ERROR: malloc(%zu) failed.\n",
                 buf_size);
         return PROC_ERR_ALLOC;
     }
 
     /* ── 4. Populate the buffer ────────────────────────────────────────── */
     memcpy(buf, data, data_len);
     buf[data_len] = '\0';             /* defensive terminator */
 
     /* ── 5. Conditionally use the buffer ──────────────────────────────── */
     if (use_buffer) {
         /*
          * Any real processing goes here.
          * We only touch `buf` inside this block — never after SECURE_FREE.
          */
         printf("[process_data] Processing %zu byte(s): ", data_len);
         for (size_t i = 0; i < data_len; i++) {
             printf("%02X ", buf[i]);
         }
         putchar('\n');
     } else {
         printf("[process_data] use_buffer=0; skipping processing.\n");
     }
 
     /* ── 6. Free — pointer is NULLed immediately by the macro ──────────── */
     SECURE_FREE(buf);
     /* buf == NULL here; any accidental dereference below would segfault
      * (a loud, detectable crash) rather than silently reading stale data. */
 
     /* Defensive assertion: catches logic errors during development. */
     /* assert(buf == NULL);   // uncomment when <assert.h> is included */
 
     return PROC_OK;
 }
 
 /* -----------------------------------------------------------------------
  * Demo driver
  * ----------------------------------------------------------------------- */
 int main(void)
 {
     const unsigned char sample[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE };
     ProcResult rc;
 
     puts("=== Test 1: use_buffer = 1 ===");
     rc = process_data(sample, sizeof(sample), 1);
     printf("Return code: %d\n\n", rc);
 
     puts("=== Test 2: use_buffer = 0 ===");
     rc = process_data(sample, sizeof(sample), 0);
     printf("Return code: %d\n\n", rc);
 
     puts("=== Test 3: NULL pointer (should error) ===");
     rc = process_data(NULL, sizeof(sample), 1);
     printf("Return code: %d\n\n", rc);
 
     puts("=== Test 4: zero length (should error) ===");
     rc = process_data(sample, 0, 1);
     printf("Return code: %d\n\n", rc);
 
     puts("=== Test 5: oversized length (should error) ===");
     rc = process_data(sample, MAX_BUFFER_SIZE + 1, 1);
     printf("Return code: %d\n\n", rc);
 
     return 0;
 }