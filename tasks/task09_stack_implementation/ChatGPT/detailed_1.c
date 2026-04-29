#include <stdio.h>
#include <stdbool.h>

#define INT_STACK_CAPACITY 32

typedef struct {
    int data[INT_STACK_CAPACITY];
    int top;  // Index of the next free slot
} IntStack;

void IntStack_init(IntStack *stack) {
    stack->top = 0;
}

bool IntStack_push(IntStack *stack, int value) {
    if (stack->top >= INT_STACK_CAPACITY) {
        return false; // Stack is full
    }

    stack->data[stack->top] = value;
    stack->top++;

    return true;
}

bool IntStack_pop(IntStack *stack, int *out_value) {
    if (stack->top <= 0) {
        return false; // Stack is empty
    }

    stack->top--;
    *out_value = stack->data[stack->top];

    return true;
}

int main(void) {
    IntStack stack;
    IntStack_init(&stack);

    IntStack_push(&stack, 10);
    IntStack_push(&stack, 20);
    IntStack_push(&stack, 30);

    int value;

    while (IntStack_pop(&stack, &value)) {
        printf("%d\n", value);
    }

    return 0;
}