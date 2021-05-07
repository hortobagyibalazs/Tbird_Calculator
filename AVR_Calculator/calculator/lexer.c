#include <stdlib.h>
#include <stdio.h>

#include "lexer.h"
#include "../util/tokens.h"

int lex(const char* exp, Token** tokens, size_t tokens_max_size, size_t* tokens_actual_size)
{
	int token_num = -1;
	TokenType prev = NONE;
	int token_data_index = 0;

	for (int i = 0; exp[i] != NULL; i++)
	{
		char c = exp[i];
		if (c == ' ') // whitespaces are ignored
		{
			continue;
		}
		else if (c == ',') // if the user tries to use a comma as a decimal separator, it has to be replaced with a period
		{
			c = '.';
		}

		TokenType current;

		switch (c)
		{
		case '+':
			current = PLUS;
			break;
		case '-':
			current = BINARY_MINUS;
			break;
		case '*':
			current = MUL;
			break;
		case '/':
			current = DIV;
			break;
		case '^':
			current = POW;
			break;
		case '(':
			current = LPAR;
			break;
		case ')':
			current = RPAR;
			break;
		default:
			current = NONE;
		}

		if (current == NONE && ((c >= '0' && c <= '9') || c == '.')) // decimal separator
		{
			current = NUMBER;
		}
		else if (current == NONE)
		{
			current = IDENT;
		}
		else if (current == BINARY_MINUS && (prev == NONE || prev == LPAR || prev == MUL || prev == DIV))
		{
			current = UNARY_MINUS;
		}

		// this is for handling tokens that are made up of more than 1 character i.e: '82', '7.4', '267', 'sqrt'
		if (!((current == NUMBER && prev == NUMBER) || (current == IDENT && prev == IDENT)))
		{
			token_num++;
			token_data_index = 0;

			if (token_num < tokens_max_size)
			{
				tokens[token_num] = (Token*)malloc(sizeof(Token));
			}
			else
			{
				return 0;
			}
		}

		if (tokens[token_num] != NULL)
		{
			tokens[token_num]->type = current;
			tokens[token_num]->word_length = token_data_index + 1;
			tokens[token_num]->data[token_data_index++] = c;
		}
		else
		{
			return 0;
		}

		prev = current;
	}

	*tokens_actual_size = token_num + 1;
	
	return 1;
}