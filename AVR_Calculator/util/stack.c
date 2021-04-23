#include "stack.h"
#include "tokens.h"
#include <stdlib.h>
#include <stdio.h>

Stack* st_init()
{
	Stack* st = (Stack*) malloc(sizeof(Stack));

	if (st != NULL)
	{
		st->capacity = STACK_CAPACITY;
		st->items_inside = 0;
	}

	return st;
}

int st_push(Stack* st, Token* item)
{
	if (st != NULL && st->items_inside < st->capacity)
	{
		st->array[st->items_inside++] = item;
		return 1;
	}
	else
	{
		return 0;
	}
}

int st_pop(Stack* st, Token** popped)
{
	if (st != NULL && st->items_inside > 0)
	{
		*popped = st->array[--st->items_inside];
		return 1;
	}
	else
	{
		return 0;
	}
}

Token* st_peek(Stack* st)
{
	return st->array[st->items_inside - 1];
}