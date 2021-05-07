#include <stdio.h>

#include "preprocessor.h"
#include "memory.h"

static int str_equals(const char* s1, const char* s2, int len)
{
	int i = 0;
	while (i < len && *(s1 + i) == *(s2 + i))
	{
		i++;
	}

	return i >= len;
}

void preprocess_tokens(Token** tokens, size_t size)
{
	for (int i = 0; i < size; i++)
	{
		// Convert constants into numbers
		if (tokens[i]->type == IDENT)
		{
			if (tokens[i]->word_length == 1 && tokens[i]->data[0] == 'e')
			{
				tokens[i]->type = NUMBER;
				int len = sprintf(tokens[i]->data, "%f", CONST_EULERS_NUM);
				tokens[i]->word_length = len;
			}
			else if (tokens[i]->word_length == 2 && str_equals("pi", tokens[i]->data, 2))
			{
				tokens[i]->type = NUMBER;
				int len = sprintf(tokens[i]->data, "%f", CONST_PI);
				tokens[i]->word_length = len;
			}
			else if (tokens[i]->word_length == 3 && str_equals("ans", tokens[i]->data, 3))
			{
				tokens[i]->type = NUMBER;
				double val = retrieve_last_result();
				int len = sprintf(tokens[i]->data, "%f", val);
				tokens[i]->word_length = len;
			}
		}
	}
}