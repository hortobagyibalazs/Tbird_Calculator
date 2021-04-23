#include <stdio.h>

#include "preprocessor.h"

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
				int len = sprintf(tokens[i]->data, "%lf", CONST_EULERS_NUM);
				tokens[i]->word_length = len;
			}
			else if (tokens[i]->word_length == 2 && tokens[i]->data[0] == 'p' && tokens[i]->data[1] == 'i')
			{
				tokens[i]->type = NUMBER;
				int len = sprintf(tokens[i]->data, "%lf", CONST_PI);
				tokens[i]->word_length = len;
			}
		}
	}
}