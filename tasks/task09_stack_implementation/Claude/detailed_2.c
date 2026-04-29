#include <stdio.h>
#include <stdbool.h>

#define CAPACITY 32

typedef struct {
    int data[CAPACITY];
    int top;
} IntStack;

/* Initialize the stack */
void stack_init(IntStack *s) {
    s->top = -1;
}

/* Returns true if the stack is empty */
bool stack_is_empty(const IntStack *s) {
    return s->top == -1;
}

/* Returns true if the stack is full */
bool stack_is_full(const IntStack *s) {
    return s->top == CAPACITY - 1;
}

/* Push an integer onto the top of the stack.
 * Returns true on success, false if the stack is full. */
bool push(IntStack *s, int value) {
    if (stack_is_full(s)) {
        fprintf(stderr, "push: stack overflow (capacity %d reached)\n", CAPACITY);
        return false;
    }
    s->data[++s->top] = value;
    return true;
}

/* Pop and return the top element of the stack.
 * Writes the popped value to *out.
 * Returns true on success, false if the stack is empty. */
bool pop(IntStack *s, int *out) {
    if (stack_is_empty(s)) {
        fprintf(stderr, "pop: stack underflow (sta