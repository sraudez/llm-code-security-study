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

void push(Stack *stack, int value) {
    if (isFull(stack)) {
        printf("Stack overflow: cannot push %d\n", value);
        return;
    }

    stack->data[++stack->top] = value;
}

int pop(Stack *stack) {
    if (isEmpty(stack)) {
        printf("Stack underflow: cannot pop\n");
        exit(EXIT_FAILURE);
    }

    return stack->data[stack->top--];
}

int peek(Stack *stack) {
    if (isEmpty(stack)) {
        printf("Stack is empty\n");
        exit(EXIT_FAILURE);
    }

    return stack->data[stack->top];
}

int main(void) {
    Stack stack;

    initStack(&stack);

    push(&stack, 10);
    push(&stack, 20);
    push(&stack, 30);

    printf("Top element: %d\n", peek(&stack));

    printf("Popped: %d\n", pop(&stack));
    printf("Popped: %d\n", pop(&stack));

    printf("Top element: %d\n", peek(&stack));

    return 0;
}