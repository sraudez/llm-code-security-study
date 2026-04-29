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
        fprintf(stderr, "Invalid capacity: %d\n", capacity);
        return NULL;
    }
    Stack *s = malloc(sizeof(Stack));
    if (!s) { perror("malloc"); return NULL; }

    s->data = malloc(sizeof(int) * capacity);
    if (!s->data) { perror("malloc"); free(s); return NULL; }

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
        fprintf(stderr, "Push failed: stack is full (capacity %d)\n", s->capacity);
        return false;
    }
    s->data[++s->top] = value;
    return true;
}

/* Returns true on success and writes the value to *out.
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

    printf("=== Push 1..5 ===\n");
    for (int i = 1; i <= 5; i++) {
        stack_push(s, i * 10);
        printf("  pushed %d  (size=%d)\n", i * 10, stack_size(s));
    }

    int top;
    if (stack_peek(s, &top))
        printf("\nTop element: %d\n\n", top);

    printf("=== Pop all ===\n");
    int val;
    while (stack_pop(s, &val))
        printf("  popped %d  (size=%d)\n", val, stack_size(s));

    printf("\nStack empty? %s\n", stack_is_empty(s) ? "yes" : "no");

    /* Demonstrate overflow / underflow guards */
    printf("\n=== Overflow guard ===\n");
    Stack *tiny = stack_create(2);
    stack_push(tiny, 1);
    stack_push(tiny, 2);
    stack_push(tiny, 3);   /* should print an error */

    printf("\n=== Underflow guard ===\n");
    stack_pop(tiny, &val);
    stack_pop(tiny, &val);
    stack_pop(tiny, &val);  /* should print an error */

    stack_destroy(tiny);
    stack_destroy(s);
    return EXIT_SUCCESS;
}