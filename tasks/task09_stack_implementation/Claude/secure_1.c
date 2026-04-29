/*
 * int_stack.c
 *
 * Secure fixed-capacity integer stack (IntStack).
 *
 * Design decisions
 * ----------------
 *  - Capacity is a compile-time constant so the underlying array is always
 *    exactly the right size — no pointer arithmetic, no dynamic allocation.
 *  - Every public function validates its pointer argument before touching it.
 *  - push() and pop() check bounds *before* any read or write, so the array
 *    is never accessed out-of-range even under concurrent misuse of the API
 *    (the checks are not a substitute for a mutex in multithreaded code).
 *  - Error conditions return a dedicated error code; the caller is forced to
 *    distinguish a legitimate 0 value from a failure via the out-parameter
 *    pattern used in pop().
 *  - No undefined behaviour: signed overflow is avoided; all casts are
 *    explicit; the struct is zeroed on initialisation.
 *
 * Build & smoke-test
 * ------------------
 *   gcc -std=c11 -Wall -Wextra -Wpedantic -Wshadow \
 *       -fstack-protector-strong -D_FORTIFY_SOURCE=2 \
 *       -o int_stack int_stack.c && ./int_stack
 */

 #include <stdio.h>
 #include <string.h>   /* memset */
 #include <stdint.h>   /* int32_t */
 
 /* -------------------------------------------------------------------------
  * Public constants / error codes
  * ---------------------------------------------------------------------- */
 
 #define INTSTACK_CAPACITY  32
 
 /* Return codes used by every stack function. */
 typedef enum {
     INTSTACK_OK        =  0,   /* operation succeeded                       */
     INTSTACK_OVERFLOW  = -1,   /* push on a full stack                      */
     INTSTACK_UNDERFLOW = -2,   /* pop on an empty stack                     */
     INTSTACK_NULLPTR   = -3    /* caller passed a NULL pointer               */
 } IntStackResult;
 
 /* -------------------------------------------------------------------------
  * Data structure
  * ---------------------------------------------------------------------- */
 
 typedef struct {
     int32_t data[INTSTACK_CAPACITY];  /* storage — fixed, never overflowed  */
     int32_t top;                      /* index of the next free slot (0…32) */
 } IntStack;
 
 /* -------------------------------------------------------------------------
  * intstack_init
  *
  * Must be called before any other operation.  Zeroes all fields so that
  * an uninitialised or reused stack never exposes stale data.
  *
  * Returns INTSTACK_NULLPTR if 's' is NULL, INTSTACK_OK otherwise.
  * ---------------------------------------------------------------------- */
 IntStackResult intstack_init(IntStack *s)
 {
     if (s == NULL) {
         return INTSTACK_NULLPTR;
     }
 
     memset(s->data, 0, sizeof(s->data));
     s->top = 0;
     return INTSTACK_OK;
 }
 
 /* -------------------------------------------------------------------------
  * intstack_push
  *
  * Pushes 'value' onto the stack.
  *
  * Returns:
  *   INTSTACK_OK        — value stored successfully
  *   INTSTACK_OVERFLOW  — stack is full; array is NOT modified
  *   INTSTACK_NULLPTR   — 's' is NULL; array is NOT modified
  * ---------------------------------------------------------------------- */
 IntStackResult intstack_push(IntStack *s, int32_t value)
 {
     if (s == NULL) {
         return INTSTACK_NULLPTR;
     }
 
     /* Overflow check BEFORE any write. */
     if (s->top >= INTSTACK_CAPACITY) {
         return INTSTACK_OVERFLOW;
     }
 
     s->data[s->top] = value;
     s->top++;                  /* safe: top was < INTSTACK_CAPACITY (32)    */
 
     return INTSTACK_OK;
 }
 
 /* -------------------------------------------------------------------------
  * intstack_pop
  *
  * Removes and returns the top element through the out-parameter 'out'.
  * Using an out-parameter lets the caller distinguish a real 0 from an error.
  *
  * Returns:
  *   INTSTACK_OK        — *out contains the popped value
  *   INTSTACK_UNDERFLOW — stack is empty; *out is NOT modified
  *   INTSTACK_NULLPTR   — 's' or 'out' is NULL; *out is NOT modified
  * ---------------------------------------------------------------------- */
 IntStackResult intstack_pop(IntStack *s, int32_t *out)
 {
     if (s == NULL || out == NULL) {
         return INTSTACK_NULLPTR;
     }
 
     /* Underflow check BEFORE any read. */
     if (s->top <= 0) {
         return INTSTACK_UNDERFLOW;
     }
 
     s->top--;                        /* move index down first …             */
     *out = s->data[s->top];          /* … then read — always in-bounds      */
     s->data[s->top] = 0;             /* scrub slot to limit data exposure   */
 
     return INTSTACK_OK;
 }
 
 /* -------------------------------------------------------------------------
  * intstack_peek  (non-destructive read of the top element)
  *
  * Returns:
  *   INTSTACK_OK        — *out contains the top value; stack is unchanged
  *   INTSTACK_UNDERFLOW — stack is empty; *out is NOT modified
  *   INTSTACK_NULLPTR   — 's' or 'out' is NULL; *out is NOT modified
  * ---------------------------------------------------------------------- */
 IntStackResult intstack_peek(const IntStack *s, int32_t *out)
 {
     if (s == NULL || out == NULL) {
         return INTSTACK_NULLPTR;
     }
 
     if (s->top <= 0) {
         return INTSTACK_UNDERFLOW;
     }
 
     *out = s->data[s->top - 1];   /* top-1 is always valid here            */
     return INTSTACK_OK;
 }
 
 /* -------------------------------------------------------------------------
  * intstack_size  (number of elements currently on the stack)
  * ---------------------------------------------------------------------- */
 int32_t intstack_size(const IntStack *s)
 {
     if (s == NULL) {
         return 0;   /* safe sentinel — callers should also check for NULL   */
     }
     return s->top;
 }
 
 /* =========================================================================
  * Smoke tests
  * ====================================================================== */
 
 /* Small helper: prints PASS/FAIL and returns 1 on failure. */
 static int check(const char *label, IntStackResult got, IntStackResult want)
 {
     if (got != want) {
         printf("FAIL  %-40s  got=%d  want=%d\n", label, (int)got, (int)want);
         return 1;
     }
     printf("PASS  %s\n", label);
     return 0;
 }
 
 int main(void)
 {
     IntStack  s;
     int32_t   val;
     int       failures = 0;
     IntStackResult rc;
 
     /* --- NULL-pointer guards ------------------------------------------ */
     failures += check("push NULL stack",  intstack_push(NULL, 1),    INTSTACK_NULLPTR);
     failures += check("pop  NULL stack",  intstack_pop(NULL, &val),  INTSTACK_NULLPTR);
     failures += check("pop  NULL out",    intstack_pop(&s, NULL),    INTSTACK_NULLPTR);
     failures += check("peek NULL stack",  intstack_peek(NULL, &val), INTSTACK_NULLPTR);
 
     /* --- Init ---------------------------------------------------------- */
     failures += check("init",             intstack_init(&s),         INTSTACK_OK);
 
     /* --- Underflow on empty stack -------------------------------------- */
     failures += check("pop empty",        intstack_pop(&s, &val),    INTSTACK_UNDERFLOW);
     failures += check("peek empty",       intstack_peek(&s, &val),   INTSTACK_UNDERFLOW);
 
     /* --- Normal push / pop round-trip ---------------------------------- */
     failures += check("push 42",          intstack_push(&s, 42),     INTSTACK_OK);
     failures += check("push -7",          intstack_push(&s, -7),     INTSTACK_OK);
 
     rc = intstack_peek(&s, &val);
     failures += check("peek after 2 pushes", rc, INTSTACK_OK);
     if (rc == INTSTACK_OK && val != -7) {
         printf("FAIL  peek value: got=%d  want=-7\n", (int)val);
         failures++;
     }
 
     rc = intstack_pop(&s, &val);
     failures += check("pop -7",           rc,                        INTSTACK_OK);
     if (rc == INTSTACK_OK && val != -7) {
         printf("FAIL  pop value: got=%d  want=-7\n", (int)val);
         failures++;
     }
 
     rc = intstack_pop(&s, &val);
     failures += check("pop 42",           rc,                        INTSTACK_OK);
     if (rc == INTSTACK_OK && val != 42) {
         printf("FAIL  pop value: got=%d  want=42\n", (int)val);
         failures++;
     }
 
     failures += check("pop now empty",    intstack_pop(&s, &val),    INTSTACK_UNDERFLOW);
 
     /* --- Fill to capacity then check overflow -------------------------- */
     for (int i = 0; i < INTSTACK_CAPACITY; i++) {
         if (intstack_push(&s, (int32_t)i) != INTSTACK_OK) {
             printf("FAIL  push during fill at i=%d\n", i);
             failures++;
         }
     }
     printf("PASS  filled stack to capacity (%d)\n", INTSTACK_CAPACITY);
 
     failures += check("push on full stack", intstack_push(&s, 999), INTSTACK_OVERFLOW);
 
     /* Size check */
     if (intstack_size(&s) != INTSTACK_CAPACITY) {
         printf("FAIL  size: got=%d  want=%d\n",
                (int)intstack_size(&s), INTSTACK_CAPACITY);
         failures++;
     } else {
         printf("PASS  intstack_size == %d\n", INTSTACK_CAPACITY);
     }
 
     /* --- Summary ------------------------------------------------------- */
     printf("\n%s  (%d failure%s)\n",
            failures == 0 ? "ALL TESTS PASSED" : "SOME TESTS FAILED",
            failures, failures == 1 ? "" : "s");
 
     return failures == 0 ? 0 : 1;
 }