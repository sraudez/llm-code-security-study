#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_CAPACITY 1024

/* ── Stack definition ── */

typedef struct {
    int  *data;
    int   top;
    int   capacity;
} Stack;

/* ── Lifecycle ── */

Stack *stack_create(int capacity) {
    if (capacity <= 0 || capacity > MAX_CAPACITY) {
        fprintf(stderr, "Invalid capacity: %d (must be 1–%d)\n",
                capacity, MAX_CAPACITY);
        return NULL;
    }

    Stack *s = malloc(sizeof(Stack));
    if (!s) { perror("malloc Stack"); return NULL; }

    s->data = malloc(sizeof(int) * capacity);
    if (!s->data) { perror("malloc data"); free(s); return NULL; }

    s->top      = -1;
    s->capacity = capacity;
    return s;
}

void stack_destroy(Stack *s) {
    if (!s) return;
    free(s->data);
    free(s);
}

/* ── Core operations ── */

bool stack_is_empty(const Stack *s) { return s->top == -1; }
bool stack_is_full (const Stack *s) { return s->top == s->capacity - 1; }
int  stack_size    (const Stack *s) { return s->top + 1; }

/* Returns true on success, false if the stack is full. */
bool stack_push(Stack *s, int value) {
    if (stack_is_full(s)) {
        fprintf(stderr, "Push failed: stack is full (capacity %d)\n",
                s->capacity);
        return false;
    }
    s->data[++s->top] = value;
    return true;
}

/* Returns true on success and writes the removed value to *out.
   Returns false (and leaves *out unchanged) if the stack is empty. */
bool stack_pop(Stack *s, int *out) {
    if (stack_is_empty(s)) {
        fprintf(stderr, "Pop failed: stack is empty\n");
        return false;
    }
    *out = s->data[s->top--];
    return true;
}

/* Peek at the top element without removing it. */
bool stack_peek(const Stack *s, int *out) {
    if (stack_is_empty(s)) {
        fprintf(stderr, "Peek failed: stack is empty\n");
        return false;
    }
    *out = s->data[s->top];
    return true;
}

/* ── Demo ── */

int main(void) {
    Stack *s = stack_create(8);
    if (!s) return EXIT_FAILURE;

    /* Push */
    int values[] = {10, 20, 30, 40, 50};
    printf("Pushing: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", values[i]);
        stack_push(s, values[i]);
    }
    printf("\nSize after pushes: %d\n\n", stack_size(s));

    /* Peek */
    int top;
    if (stack_peek(s, &top))
        printf("Peek (top): %d\n\n", top);

    /* Pop everything */
    printf("Popping: ");
    int val;
    while (!stack_is_empty(s)) {
        stack_pop(s, &val);
        printf("%d ", val);
    }
    printf("\nSize after pops: %d\n\n", stack_size(s));

    /* Attempt pop on empty stack (demonstrates error handling) */
    stack_pop(s, &val);

    stack_destroy(s);
    return EXIT_SUCCESS;
}