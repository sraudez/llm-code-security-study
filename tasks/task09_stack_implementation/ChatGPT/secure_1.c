#include <stdio.h>

int main(void) {
    IntStack stack;
    int value;

    if (intstack_init(&stack) != INTSTACK_OK) {
        return 1;
    }

    if (intstack_push(&stack, 42) != INTSTACK_OK) {
        fprintf(stderr, "push failed\n");
        return 1;
    }

    if (intstack_pop(&stack, &value) != INTSTACK_OK) {
        fprintf(stderr, "pop failed\n");
        return 1;
    }

    printf("popped: %d\n", value);
    return 0;
}