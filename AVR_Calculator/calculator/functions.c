/*
 * functions.c
 *
 * Created: 4/27/2021 9:14:02 PM
 *  Author: Dell
 */ 
#include "functions.h"
#include <math.h>

static int str_equals(const char* s1, const char* s2, int len)
{
	int i = 0;
	while (i < len && *(s1 + i) == *(s2 + i))
	{
		i++;
	}

	return i >= len;
}

int evaluate_function(const Token* _operator, const Token* operand, double* result)
{
	char data[operand->word_length + 1];
	data[operand->word_length] = '\0';
	memcpy(data, operand->data, sizeof(char) * operand->word_length);
	
	double operand_val = strtod(data, NULL);
	
	if (_operator->type == UNARY_MINUS)
	{
		*result = -1 * operand_val;
	}
	else if (_operator->type != IDENT)
	{
		return 0;
	}
	else
	{
		char* fun_name = _operator->data;
		int len = _operator->word_length;
		
		if (len == 4 && str_equals(fun_name, "sqrt", 4))
		{
			*result = sqrt(operand_val);
		}
		else if (len == 3 && str_equals(fun_name, "sin", 3))
		{
			*result = sin(operand_val);
		}
		else if (len == 3 && str_equals(fun_name, "cos", 3))
		{
			*result = cos(operand_val);
		}
		else
		{
			return 0;
		}
	}
	
	return 1;
}
