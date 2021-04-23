#include <stdlib.h>

#include "tokens.h"

void tk_array_free(Token** tokens, size_t size)
{
	for (int i = 0; i < size; i++)
	{
		if (tokens[i] && tokens[i]->data != NULL)
		{
			free(tokens[i]);
			tokens[i] = NULL;
		}
	}
}