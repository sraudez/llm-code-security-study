#include <stdio.h>
#include <stdbool.h>

#define INT_STACK_CAPACITY 32

typedef struct {
    int data[INT_STACK_CAPACITY];
    int top; // Index of the next free slot; 0 means empty
} IntStack;

/* Initialise a stack so it is ready for use. */
void int_stack_init(IntStack *s) {
    s->top = 0;
}

/* Returns true when the stack holds no elements. */
bool int_stack_is_empty(const IntStack *s) {
    return s->top == 0;
}

/* Returns true when the stack has no room for additional elements. */
bool int_stack_is_full(const IntStack *s) {
    return s->top == INT_STACK_CAPACITY;
}

/*
 * Push an integer onto the stack.
 * Returns true on success, or false if the stack is already full.
 */
bool push(IntStack *s, int value) {
    if (int_stack_is_full(s)) {
        fprintf(stderr, "push: stack is