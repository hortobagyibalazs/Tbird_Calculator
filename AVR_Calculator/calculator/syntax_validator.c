#include "syntax_validator.h"

int is_operator(TokenType type)
{
	return type == PLUS || type == BINARY_MINUS || type == UNARY_MINUS || type == MUL || type == DIV;
}

int syntax_validate(const Token** tokens, size_t size)
{
	int unenclosed_parentheses = 0;
	TokenType prev = NONE;

	int i = 0;
	while (i < size)
	{
		TokenType type = tokens[i]->type;

		if (type == LPAR)
		{
			unenclosed_parentheses++;
		}
		else if (type == RPAR)
		{
			unenclosed_parentheses--;
		}
		else if (type == LPAR && prev == RPAR) // () is invalid
		{
			return 0;
		}
		else if (type == NUMBER && prev == NUMBER) // NUM NUM is invalid
		{
			return 0;
		}
		else if (is_operator(type) && is_operator(prev) && (type == UNARY_MINUS && (prev != MUL && prev != DIV))) // +-, */, *+, are all invalid, *- and /- are valid
		{
			return 0;
		}
		else if (prev == LPAR && is_operator(type) && type != UNARY_MINUS) // (+, (*, (/ are invalid, (- is valid
		{
			return 0;
		}
		else if (type == NUMBER) // multiple decimal separators in a number are invalid i.e: '3.76.4'
		{
			int decimal_separators = 0;
			int j = 0;
			while (j < tokens[i]->word_length)
			{
				if (tokens[i]->data[j] == '.')
				{
					decimal_separators++;
				}

				if (decimal_separators > 1)
				{
					return 0;
				}

				j++;
			}
		}
		
		prev = type;
		i++;
	}

	return unenclosed_parentheses == 0;
}