#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "../util/stack.h"
#include "exp_evaluator.h"
#include "syntax_validator.h"
#include "functions.h"

int is_left_assoc(TokenType _operator)
{
	return _operator != POW; // every operator is left associative except '^'
}

int precedence(TokenType _operator)
{
	if (_operator == POW)
	{
		return 3;
	}
	else if (_operator == UNARY_MINUS || _operator == IDENT)
	{
		return 2;
	}
	else if (_operator == MUL || _operator == DIV)
	{
		return 1;
	}
	else if (_operator == PLUS || _operator == BINARY_MINUS)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

int shunting_yard(const Token** tokens, size_t tokens_size, Token** rpn_out, size_t output_max_size, size_t* output_actual_size)
{
	int status_code = 1;

	Stack* stack = st_init();

	int i = 0;
	int buffer_index = 0;

	// 1) Iterate through tokens or until buffer overflows
	while (i < tokens_size && buffer_index < output_max_size)
	{
		// 1.1) Read next token
		const Token* token = tokens[i];
		const TokenType type = token->type;
		
		// 1.2) If token is an operator (x)
		if (is_operator(type))
		{
			// 1.2.1) While there is an operator (y) at the top of the operators stack
			// and either (x) is left-associative and its precedence is less or equal to that of (y), or (x)
			// is right-associative and its precedence is less than (y)
			while (stack->items_inside > 0 &&
			(  (is_left_assoc(type) && precedence(type) <= precedence(st_peek(stack)->type) )
			||
			(!is_left_assoc(type) && precedence(type) < precedence(st_peek(stack)->type)))  )
			{
				if (buffer_index >= output_max_size)
				{
					status_code = 0;
					break;
				}
				
				// 1.2.1.1)
				Token* popped = NULL;
				if (!st_pop(stack, &popped))
				{
					status_code = 0;
					break;
				}

				// 1.2.1.2)
				rpn_out[buffer_index++] = popped;
			}

			int success = st_push(stack, token);
			if (!success)
			{
				status_code = 0;
				break;
			}
		}
		// 1.3) Else if token is left parenthesis, then push it on the stack
		else if (type == LPAR)
		{
			int success = st_push(stack, token);
			if (!success)
			{
				status_code = 0;
				break;
			}
		}
		// 1.4) Else if token is a right parenthesis...
		else if (type == RPAR)
		{
			// 1.4.1) Until the top token (from the stack) is left parenthesis, pop from the stack to the output buffer
			while (stack->items_inside > 0 && st_peek(stack)->type != LPAR)
			{
				Token* popped = NULL;
				if (!st_pop(stack, &popped) || buffer_index >= output_max_size - 1)
				{
					status_code = 0;
					break;
				}

				rpn_out[buffer_index++] = popped;
			}

			// 1.4.2) Also pop the left parenthesis but don?t include it in the output buffer
			if (stack->items_inside > 0)
			{
				Token* popped = NULL;
				if (!st_pop(stack, &popped))
				{
					status_code = 0;
					break;
				}
			}
		}
		// 1.5) Else add token to output buffer
		else
		{
			if (buffer_index < output_max_size)
			{
				rpn_out[buffer_index++] = token;
			}
			else
			{
				status_code = 0;
				break;
			}
		}

		i++;
	}


	// 2) Pop remaining tokens and add them to the output buffer
	while (stack->items_inside > 0)
	{
		if (buffer_index >= output_max_size)
		{
			status_code = 0;
			break;
		}

		st_pop(stack, &rpn_out[buffer_index++]);
	}

	*output_actual_size = buffer_index;
	
	free(stack);
	stack = NULL;

	return status_code;
}

int evaluate_rpn(const Token** rpn, size_t size, double* result)
{
	int status_code = 1;
	Stack* stack = st_init();

	int i = 0;
	while (i < size)
	{
		const Token* token = rpn[i];
		TokenType type = token->type;

		if (type == NUMBER)
		{
			st_push(stack, token);
		}
		else if (type == UNARY_MINUS || type == IDENT)
		{
			Token* op = st_peek(stack);

			double val = 0;
			int stat = evaluate_function(token, op, &val);
			if (!stat)
			{
				status_code = 0;
				break;
			}

			char data[MAX_WORD_LENGTH];
			int len = snprintf(data, MAX_WORD_LENGTH, "%f", val);

			for (int j = 0; j < len; j++)
			{
				op->data[j] = data[j];
			}
			op->word_length = len;
			op->type = NUMBER;
		}
		else if (is_operator(type)) // operator but not unary minus/function
		{
			Token* op1 = NULL; // operands
			Token* op2 = NULL;
			st_pop(stack, &op1);
			st_pop(stack, &op2);

			char data1[op1->word_length + 1];
			char data2[op2->word_length + 1];
			
			data1[op1->word_length] = '\0';
			data2[op2->word_length] = '\0';
			
			memcpy(data1, op1->data, sizeof(char) * op1->word_length);
			memcpy(data2, op2->data, sizeof(char) * op2->word_length);

			double val1 = strtod(data1, NULL);
			double val2 = strtod(data2, NULL);

			double partial_result = 0;

			if (type == PLUS)
			{
				partial_result = val1 + val2;
			}
			else if (type == BINARY_MINUS)
			{
				partial_result = val2 - val1;
			}
			else if (type == MUL)
			{
				partial_result = val1 * val2;
			}
			else if (type == DIV)
			{
				if (val1 == 0)
				{
					status_code = 0;
					break;
				}
				partial_result = val2 / val1;
			}
			else if (type == POW)
			{
				// val1 - exponent
				partial_result = pow(val2, val1);
			}

			char data[MAX_WORD_LENGTH];
			int len = snprintf(data, MAX_WORD_LENGTH, "%f", partial_result);

			for (int j = 0; j < len; j++)
			{
				op1->data[j] = data[j];
			}

			op1->word_length = len;
			op1->type = NUMBER;
			st_push(stack, op1);
		}

		i++;
	}

	if (stack->items_inside > 0)
	{
		Token* result_token = NULL;
		st_pop(stack, &result_token);
		
		char data[result_token->word_length + 1];
		data[result_token->word_length] = '\0';
		memcpy(data, result_token->data, sizeof(char) * result_token->word_length);
		
		*result = strtod(data, NULL);
	}
	
	free(stack);
	stack = NULL;

	return status_code;
}