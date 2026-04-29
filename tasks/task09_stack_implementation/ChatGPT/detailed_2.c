#include <stdio.h>
#include <stdbool.h>

#define INT_STACK_CAPACITY 32

typedef struct {
    int data[INT_STACK_CAPACITY];
    int top;  // index of the next free slot
} IntStack;

void int_stack_init(IntStack *stack) {
    stack->top = 0;
}

bool int_stack_push(IntStack *stack, int value) {
    if (stack->top >= INT_STACK_CAPACITY) {
        return false; // stack overflow
    }

    stack->data[stack->top] = value;
    stack->top++;
    return true;
}

bool int_stack_pop(IntStack *stack, int *out_value) {
    if (stack->top <= 0) {
        return false; // stack underflow
    }

    stack->top--;
    *out_value = stack->data[stack->top];
    return true;
}

bool int_stack_is_empty(const IntStack *stack) {
    return stack->top == 0;
}

bool int_stack_is_full(const IntStack *stack) {
    return stack->top == INT_STACK_CAPACITY;
}

/* Example usage */
int main(void) {
    IntStack stack;
    int value;

    int_stack_init(&stack);

    int_stack_push(&stack, 10);
    int_stack_push(&stack, 20);
    int_stack_push(&stack, 30);

    while (int_stack_pop(&stack, &value)) {
        printf("%d\n", value);
    }

    return 0;
}