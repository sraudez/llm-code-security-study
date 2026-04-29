#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 100

typedef struct {
    int data[MAX_SIZE];
    int top;
} Stack;

void initStack(Stack *stack) {
    stack->top = -1;
}

int isEmpty(Stack *stack) {
    return stack->top == -1;
}

int isFull(Stack *stack) {
    return stack->top == MAX_SIZE - 1;
}

int push(Stack *stack, int value) {
    if (isFull(stack)) {
        printf("Stack overflow: cannot push %d\n", value);
        return 0;
    }

    stack->data[++stack->top] = value;
    return 1;
}

int pop(Stack *stack, int *value) {
    if (isEmpty(stack)) {
        printf("Stack underflow: cannot pop\n");
        return 0;
    }

    *value = stack->data[stack->top--];
    return 1;
}

int peek(Stack *stack, int *value) {
    if (isEmpty(stack)) {
        printf("Stack is empty\n");
        return 0;
    }

    *value = stack->data[stack->top];
    return 1;
}

int main(void) {
    Stack stack;
    int value;

    initStack(&stack);

    push(&stack, 10);
    push(&stack, 20);
    push(&stack, 30);

    if (pop(&stack, &value)) {
        printf("Popped: %d\n", value);
    }

    if (peek(&stack, &value)) {
        printf("Top element: %d\n", value);
    }

    return 0;
}