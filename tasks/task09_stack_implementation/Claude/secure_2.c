/*
 * int_stack.c
 *
 * Secure, fixed-capacity integer stack (capacity: INT_STACK_CAPACITY elements).
 *
 * Design principles:
 *   - All bounds checked before every array access — no speculative indexing.
 *   - No undefined behaviour: no signed overflow, no out-of-bounds reads/writes.
 *   - Opaque struct: callers cannot manipulate internal fields directly.
 *   - Every function returns a typed error code; no errno side-channel reliance.
 *   - Stack state is always left consistent even on error paths.
 *   - Popped slots are zeroed to prevent stale-data leaks (defense-in-depth).
 */

 #include <stddef.h>   /* NULL, size_t          */
 #include <stdint.h>   /* int32_t, SIZE_MAX     */
 #include <string.h>   /* memset                */
 #include <stdio.h>    /* printf (demo only)    */
 
 /* -------------------------------------------------------------------------
  * Configuration
  * ---------------------------------------------------------------------- */
 
 #define INT_STACK_CAPACITY  32u   /* maximum number of elements; unsigned  */
 
 /* -------------------------------------------------------------------------
  * Error codes
  * ---------------------------------------------------------------------- */
 
 typedef enum {
     INT_STACK_OK        =  0,   /* operation succeeded                     */
     INT_STACK_OVERFLOW  = -1,   /* push on a full stack                    */
     INT_STACK_UNDERFLOW = -2,   /* pop on an empty stack                   */
     INT_STACK_NULL_PTR  = -3    /* caller passed a NULL stack pointer       */
 } IntStackStatus;
 
 /* -------------------------------------------------------------------------
  * Opaque type (definition kept in this TU; expose only via header in prod)
  * ---------------------------------------------------------------------- */
 
 typedef struct {
     int32_t data[INT_STACK_CAPACITY];   /* storage — never accessed by index
                                            without a prior bounds check       */
     size_t  top;                        /* number of live elements (0 = empty)*/
 } IntStack;
 
 /* -------------------------------------------------------------------------
  * intstack_init
  *
  * Must be called once before any other operation on a stack instance.
  * Zeroes all fields so the stack is in a defined, empty state.
  *
  * Returns: INT_STACK_NULL_PTR if s is NULL, INT_STACK_OK otherwise.
  * ---------------------------------------------------------------------- */
 IntStackStatus intstack_init(IntStack *s)
 {
     if (s == NULL) {
         return INT_STACK_NULL_PTR;
     }
 
     memset(s->data, 0, sizeof(s->data));
     s->top = 0u;
 
     return INT_STACK_OK;
 }
 
 /* -------------------------------------------------------------------------
  * intstack_push
  *
  * Pushes `value` onto the stack.
  *
  * Returns:
  *   INT_STACK_OK        — value stored successfully.
  *   INT_STACK_OVERFLOW  — stack is full; stack is unchanged.
  *   INT_STACK_NULL_PTR  — s is NULL; no side-effects.
  * ---------------------------------------------------------------------- */
 IntStackStatus intstack_push(IntStack *s, int32_t value)
 {
     if (s == NULL) {
         return INT_STACK_NULL_PTR;
     }
 
     /* Overflow check: top must be strictly less than capacity.
        Because top is size_t (unsigned) this comparison is always safe. */
     if (s->top >= INT_STACK_CAPACITY) {
         return INT_STACK_OVERFLOW;
     }
 
     /* Safe: we have verified top < INT_STACK_CAPACITY (== array bound). */
     s->data[s->top] = value;
     s->top++;                   /* cannot wrap: top < SIZE_MAX guaranteed by
                                    the capacity check above                  */
 
     return INT_STACK_OK;
 }
 
 /* -------------------------------------------------------------------------
  * intstack_pop
  *
  * Removes and returns the top element via the `out` parameter.
  *
  * Returns:
  *   INT_STACK_OK         — *out is set to the popped value.
  *   INT_STACK_UNDERFLOW  — stack is empty; *out is unchanged.
  *   INT_STACK_NULL_PTR   — s or out is NULL; no side-effects.
  * ---------------------------------------------------------------------- */
 IntStackStatus intstack_pop(IntStack *s, int32_t *out)
 {
     if (s == NULL || out == NULL) {
         return INT_STACK_NULL_PTR;
     }
 
     /* Underflow check: top == 0 means the stack is empty.
        Because top is unsigned, testing == 0 is unambiguous. */
     if (s->top == 0u) {
         return INT_STACK_UNDERFLOW;
     }
 
     s->top--;                           /* decrement before indexing          */
 
     /* Safe: top was > 0 before decrement, so 0 <= top < INT_STACK_CAPACITY. */
     *out = s->data[s->top];
 
     /* Zero the vacated slot to prevent stale data from lingering in memory. */
     s->data[s->top] = 0;
 
     return INT_STACK_OK;
 }
 
 /* -------------------------------------------------------------------------
  * intstack_peek  (non-destructive read of the top element)
  *
  * Returns:
  *   INT_STACK_OK         — *out is set to the top value; stack unchanged.
  *   INT_STACK_UNDERFLOW  — stack is empty; *out is unchanged.
  *   INT_STACK_NULL_PTR   — s or out is NULL; no side-effects.
  * ---------------------------------------------------------------------- */
 IntStackStatus intstack_peek(const IntStack *s, int32_t *out)
 {
     if (s == NULL || out == NULL) {
         return INT_STACK_NULL_PTR;
     }
 
     if (s->top == 0u) {
         return INT_STACK_UNDERFLOW;
     }
 
     /* Safe: top > 0, so top - 1 < INT_STACK_CAPACITY. */
     *out = s->data[s->top - 1u];
 
     return INT_STACK_OK;
 }
 
 /* -------------------------------------------------------------------------
  * intstack_size  (returns element count via out-parameter)
  *
  * Returns: INT_STACK_NULL_PTR if either pointer is NULL, INT_STACK_OK
  * otherwise.  *out is in [0, INT_STACK_CAPACITY].
  * ---------------------------------------------------------------------- */
 IntStackStatus intstack_size(const IntStack *s, size_t *out)
 {
     if (s == NULL || out == NULL) {
         return INT_STACK_NULL_PTR;
     }
 
     *out = s->top;
 
     return INT_STACK_OK;
 }
 
 /* =========================================================================
  * Self-contained demonstration / manual test
  * ====================================================================== */
 
 static const char *status_str(IntStackStatus st)
 {
     switch (st) {
         case INT_STACK_OK:        return "OK";
         case INT_STACK_OVERFLOW:  return "OVERFLOW";
         case INT_STACK_UNDERFLOW: return "UNDERFLOW";
         case INT_STACK_NULL_PTR:  return "NULL_PTR";
         default:                  return "UNKNOWN";
     }
 }
 
 int main(void)
 {
     IntStack stk;
     IntStackStatus st;
     int32_t  val;
     size_t   sz;
 
     /* --- init ---------------------------------------------------------- */
     st = intstack_init(&stk);
     printf("init              → %s\n", status_str(st));
 
     /* --- push 32 values (fills the stack exactly) ---------------------- */
     for (int32_t i = 1; i <= (int32_t)INT_STACK_CAPACITY; i++) {
         st = intstack_push(&stk, i * 10);
         if (st != INT_STACK_OK) {
             printf("push(%d)  → %s  [UNEXPECTED]\n", i * 10, status_str(st));
         }
     }
     intstack_size(&stk, &sz);
     printf("after 32 pushes, size = %zu\n", sz);
 
     /* --- push one more (must overflow) --------------------------------- */
     st = intstack_push(&stk, 9999);
     printf("push(9999) on full → %s\n", status_str(st));   /* expect OVERFLOW */
 
     /* --- peek ---------------------------------------------------------- */
     st = intstack_peek(&stk, &val);
     printf("peek              → %s, value = %d\n", status_str(st), val);
 
     /* --- pop all 32 values --------------------------------------------- */
     for (size_t i = 0; i < INT_STACK_CAPACITY; i++) {
         st = intstack_pop(&stk, &val);
         if (st != INT_STACK_OK) {
             printf("pop[%zu]  → %s  [UNEXPECTED]\n", i, status_str(st));
         }
     }
     intstack_size(&stk, &sz);
     printf("after 32 pops,  size = %zu\n", sz);
 
     /* --- pop one more (must underflow) --------------------------------- */
     st = intstack_pop(&stk, &val);
     printf("pop on empty      → %s\n", status_str(st));    /* expect UNDERFLOW */
 
     /* --- NULL-pointer guard -------------------------------------------- */
     st = intstack_push(NULL, 42);
     printf("push(NULL, 42)    → %s\n", status_str(st));    /* expect NULL_PTR  */
 
     st = intstack_pop(NULL, &val);
     printf("pop(NULL, &val)   → %s\n", status_str(st));    /* expect NULL_PTR  */
 
     st = intstack_pop(&stk, NULL);
     printf("pop(&stk, NULL)   → %s\n", status_str(st));    /* expect NULL_PTR  */
 
     return 0;
 }