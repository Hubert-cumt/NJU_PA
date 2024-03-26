#ifndef FUNCSTACK_H

#define FUNCSTACK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <common.h>

#define MAX_STACK_SIZE 128

typedef struct {
    char name[32];
    word_t entry;
    word_t addr;
} Pair;

typedef struct {
    Pair data[MAX_STACK_SIZE]; 
    word_t entry;
    int top;
} Stack;

Stack* stack_init();

void stack_del(Stack* st);

int stack_isEmpty(Stack* stack);

int stack_isFull(Stack* stack);

void stack_push(Stack* stack, const char* name, word_t entry, word_t addr);

Pair stack_pop(Stack* stack);

Pair stack_top(Stack* stack);
#endif
