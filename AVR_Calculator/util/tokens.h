#pragma once

#define MAX_TOKENS 64
#define MAX_WORD_LENGTH 16

#include <stdlib.h>

typedef enum TokenType
{
	NUMBER,
	PLUS,
	BINARY_MINUS,
	UNARY_MINUS,
	MUL,
	DIV,
	LPAR,
	RPAR,
	IDENT, // could be a function's name or a constant
	NONE // not yet determined
} TokenType;

typedef struct Token
{
	char data[MAX_WORD_LENGTH];
	size_t word_length;
	TokenType type;
} Token;

void tk_array_free(Token** tokens, size_t size);