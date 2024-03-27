#include <dataStructure/funcStack.h>

Stack* stack_init() {
    Stack* st = malloc(sizeof(Stack));
    st->top = -1;
    return st;
}

void stack_del(Stack* st) {
    free(st);
}

int stack_isEmpty(Stack* stack) {
    return stack->top == -1;
}

int stack_isFull(Stack* stack) {
    return stack->top == MAX_STACK_SIZE - 1;
}

void stack_push(Stack* stack, const char* name, word_t entry, word_t addr) {
    if (stack_isFull(stack)) {
        printf("Error: Stack is full.\n");
        return;
    }
    Pair pair;
    strncpy(pair.name, name, sizeof(pair.name) - 1);  
    pair.name[sizeof(pair.name) - 1] = '\0'; 
    pair.addr = addr;
    pair.entry = entry;
    stack->data[++stack->top] = pair;
}

// Pair stack_pop(Stack* stack) {
//     Pair pair;
//     if (stack_isEmpty(stack)) {
//         printf("Error: Stack is empty.\n");
//         exit(EXIT_FAILURE);
//     }
//     pair = stack->data[stack->top--];
//     return pair;
// }

void stack_pop(Stack* stack) {
    if (stack_isEmpty(stack)) {
        printf("Error: Stack is empty.\n");
        exit(EXIT_FAILURE);
    }
    stack->top--;
}

Pair stack_top(Stack* stack) {
    Pair pair;
    if (stack_isEmpty(stack)) {
        printf("Error: Stack is empty.\n");
        exit(EXIT_FAILURE);
    }
    pair = stack->data[stack->top];
    return pair;
}