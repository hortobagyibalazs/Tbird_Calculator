#pragma once

#include <stdio.h>
#include "tokens.h"
#define STACK_CAPACITY 64

typedef struct stack {
	size_t capacity;
	size_t items_inside;
	Token* array[STACK_CAPACITY];
} Stack;

Stack* st_init();

// Returns exit code of the function
// 0: failure
// 1: success
int st_push(Stack* st, Token* item);

// Returns exit code of the function
// 0: failure
// 1: success
int st_pop(Stack* st, Token** popped);

// Returns the top element of the stack without popping it
Token* st_peek(Stack* st);