/*
 * Secure fixed-capacity integer stack.
 *
 * Capacity: 32 int elements
 * Push failure: returns INTSTACK_ERR_OVERFLOW
 * Pop failure:  returns INTSTACK_ERR_UNDERFLOW
 */

 #include <stddef.h>
 #include <stdbool.h>
 
 #define INTSTACK_CAPACITY 32
 
 typedef enum {
     INTSTACK_OK = 0,
     INTSTACK_ERR_NULL = -1,
     INTSTACK_ERR_OVERFLOW = -2,
     INTSTACK_ERR_UNDERFLOW = -3
 } IntStackStatus;
 
 typedef struct {
     int data[INTSTACK_CAPACITY];
     size_t size;
 } IntStack;
 
 /*
  * Initialize the stack before use.
  */
 IntStackStatus intstack_init(IntStack *stack)
 {
     if (stack == NULL) {
         return INTSTACK_ERR_NULL;
     }
 
     stack->size = 0;
     return INTSTACK_OK;
 }
 
 /*
  * Returns true if the stack is empty.
  */
 bool intstack_is_empty(const IntStack *stack)
 {
     return stack == NULL || stack->size == 0;
 }
 
 /*
  * Returns true if the stack is full.
  */
 bool intstack_is_full(const IntStack *stack)
 {
     return stack != NULL && stack->size >= INTSTACK_CAPACITY;
 }
 
 /*
  * Pushes a value onto the stack.
  *
  * This function checks for overflow before writing.
  * It never writes past the bounds of stack->data.
  */
 IntStackStatus intstack_push(IntStack *stack, int value)
 {
     if (stack == NULL) {
         return INTSTACK_ERR_NULL;
     }
 
     if (stack->size >= INTSTACK_CAPACITY) {
         return INTSTACK_ERR_OVERFLOW;
     }
 
     stack->data[stack->size] = value;
     stack->size++;
 
     return INTSTACK_OK;
 }
 
 /*
  * Pops a value from the stack and stores it in *out_value.
  *
  * This function checks for underflow before reading.
  * It never reads outside the bounds of stack->data.
  */
 IntStackStatus intstack_pop(IntStack *stack, int *out_value)
 {
     if (stack == NULL || out_value == NULL) {
         return INTSTACK_ERR_NULL;
     }
 
     if (stack->size == 0) {
         return INTSTACK_ERR_UNDERFLOW;
     }
 
     stack->size--;
     *out_value = stack->data[stack->size];
 
     return INTSTACK_OK;
 }