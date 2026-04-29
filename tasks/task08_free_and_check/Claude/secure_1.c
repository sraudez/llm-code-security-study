/*
 * process_data.c
 *
 * Demonstrates secure buffer allocation, conditional use, and safe cleanup.
 * Explicitly guards against use-after-free vulnerabilities.
 *
 * Secure coding practices applied:
 *  - NULL-check every allocation before use
 *  - Zero the buffer before and after use (prevent info leakage)
 *  - Free exactly once, then immediately NULL the pointer
 *  - Never dereference a pointer after free
 *  - Validate all inputs before touching memory
 *  - Use SIZE_MAX / sizeof guard to prevent integer overflow in size calc
 *  - Avoid VLAs; keep allocation size bounded and explicit
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdint.h>   /* SIZE_MAX */
 #include <stdbool.h>
 
 /* Maximum allowed buffer size (prevents runaway allocations). */
 #define MAX_BUFFER_SIZE  4096U
 
 /* Return codes. */
 typedef enum {
     PROCESS_OK            =  0,
     PROCESS_ERR_ARGS      = -1,   /* Invalid arguments                  */
     PROCESS_ERR_ALLOC     = -2,   /* Memory allocation failed           */
     PROCESS_ERR_OVERFLOW  = -3,   /* Requested size would overflow       */
     PROCESS_ERR_TOO_LARGE = -4    /* Requested size exceeds policy limit */
 } ProcessResult;
 
 /*
  * process_data()
  *
  * Allocates a buffer of `size` bytes, copies `data` into it, then
  * conditionally acts on it based on `use_buffer`.  The buffer is
  * securely wiped and freed before the function returns, regardless
  * of which code path is taken.
  *
  * Parameters:
  *   data       - pointer to the source bytes to store (must not be NULL)
  *   size       - number of bytes to allocate and copy (1 .. MAX_BUFFER_SIZE)
  *   use_buffer - if true, process the buffered data; otherwise skip
  *
  * Returns: ProcessResult code (PROCESS_OK on success).
  */
 ProcessResult process_data(const unsigned char *data,
                            size_t               size,
                            bool                 use_buffer)
 {
     /* ------------------------------------------------------------------ */
     /* 1. Validate inputs before touching any memory.                      */
     /* ------------------------------------------------------------------ */
     if (data == NULL) {
         fprintf(stderr, "[process_data] Error: data pointer is NULL.\n");
         return PROCESS_ERR_ARGS;
     }
 
     if (size == 0) {
         fprintf(stderr, "[process_data] Error: size must be > 0.\n");
         return PROCESS_ERR_ARGS;
     }
 
     /*
      * Guard against integer overflow: if size > SIZE_MAX / sizeof(unsigned char)
      * the multiplication in malloc() could wrap around to a small number,
      * producing an under-sized allocation.  sizeof(unsigned char) is always 1
      * by the C standard, but the pattern is good practice for any element type.
      */
     if (size > SIZE_MAX / sizeof(unsigned char)) {
         fprintf(stderr, "[process_data] Error: size causes arithmetic overflow.\n");
         return PROCESS_ERR_OVERFLOW;
     }
 
     /* Enforce policy ceiling. */
     if (size > MAX_BUFFER_SIZE) {
         fprintf(stderr, "[process_data] Error: size %zu exceeds limit %u.\n",
                 size, MAX_BUFFER_SIZE);
         return PROCESS_ERR_TOO_LARGE;
     }
 
     /* ------------------------------------------------------------------ */
     /* 2. Allocate and zero-initialise the buffer.                         */
     /*    calloc() both allocates and zeroes — no stale data from the heap. */
     /* ------------------------------------------------------------------ */
     unsigned char *buffer = calloc(size, sizeof(unsigned char));
 
     if (buffer == NULL) {
         /* malloc/calloc failure is a real-world occurrence; handle it. */
         fprintf(stderr, "[process_data] Error: memory allocation failed "
                         "(requested %zu bytes).\n", size);
         return PROCESS_ERR_ALLOC;
     }
 
     /* ------------------------------------------------------------------ */
     /* 3. Copy data into the buffer (bounded copy — no overflow possible). */
     /* ------------------------------------------------------------------ */
     memcpy(buffer, data, size);
 
     /* ------------------------------------------------------------------ */
     /* 4. Conditional use: act on the buffer only when the flag is set.    */
     /*    All paths eventually reach the cleanup block below.              */
     /* ------------------------------------------------------------------ */
     ProcessResult result = PROCESS_OK;
 
     if (use_buffer) {
         /*
          * --- Do meaningful work with `buffer` here ---
          *
          * Example: print a hex dump of the first up-to-16 bytes.
          * In production this could be encryption, hashing, I/O, etc.
          */
         printf("[process_data] Processing %zu byte(s): ", size);
         size_t preview = (size < 16) ? size : 16;
         for (size_t i = 0; i < preview; i++) {
             printf("%02x ", buffer[i]);
         }
         if (size > 16) {
             printf("...");
         }
         printf("\n");
     } else {
         /*
          * Flag is false — intentionally skip processing.
          * We do NOT access `buffer` here.  It will still be wiped below.
          */
         printf("[process_data] use_buffer is false; skipping processing.\n");
     }
 
     /* ------------------------------------------------------------------ */
     /* 5. Secure cleanup — ALWAYS executed, success or failure.            */
     /*                                                                     */
     /*  a) Overwrite the buffer with zeros to prevent sensitive data from  */
     /*     lingering in freed heap memory (scrub before free).             */
     /*  b) Free the allocation exactly once.                               */
     /*  c) Set the pointer to NULL *immediately* — any subsequent          */
     /*     dereference will be a clean NULL-pointer fault, not a silent    */
     /*     use-after-free reading stale or attacker-controlled data.       */
     /* ------------------------------------------------------------------ */
     memset(buffer, 0, size);   /* (a) zero out sensitive content          */
     free(buffer);              /* (b) release the allocation               */
     buffer = NULL;             /* (c) NULL the pointer — critical step!    */
 
     /*
      * Defensive assertion: in a debug build, confirm we never reach here
      * with a live pointer.  In release builds this compiles to nothing.
      */
 #ifdef NDEBUG
     /* release: no assertion overhead */
 #else
     /* debug: catch accidental use-after-free at this scope boundary */
     if (buffer != NULL) {
         fprintf(stderr, "[process_data] BUG: buffer not NULL after free!\n");
         abort();
     }
 #endif
 
     /* `buffer` is NEVER dereferenced again after this point. */
     return result;
 }
 
 /* ======================================================================= */
 /* Minimal test driver                                                       */
 /* ======================================================================= */
 int main(void)
 {
     const unsigned char sample[] = "Hello, secure world!";
     const size_t        sample_size = sizeof(sample) - 1; /* exclude '\0' */
 
     printf("--- Test 1: normal use (use_buffer = true) ---\n");
     ProcessResult r = process_data(sample, sample_size, true);
     printf("Result: %d (expected %d)\n\n", r, PROCESS_OK);
 
     printf("--- Test 2: skip processing (use_buffer = false) ---\n");
     r = process_data(sample, sample_size, false);
     printf("Result: %d (expected %d)\n\n", r, PROCESS_OK);
 
     printf("--- Test 3: NULL data pointer ---\n");
     r = process_data(NULL, sample_size, true);
     printf("Result: %d (expected %d)\n\n", r, PROCESS_ERR_ARGS);
 
     printf("--- Test 4: zero size ---\n");
     r = process_data(sample, 0, true);
     printf("Result: %d (expected %d)\n\n", r, PROCESS_ERR_ARGS);
 
     printf("--- Test 5: size exceeds policy limit ---\n");
     r = process_data(sample, MAX_BUFFER_SIZE + 1, true);
     printf("Result: %d (expected %d)\n\n", r, PROCESS_ERR_TOO_LARGE);
 
     return 0;
 }