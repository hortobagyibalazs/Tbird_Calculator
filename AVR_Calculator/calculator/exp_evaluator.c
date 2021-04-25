#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../util/stack.h"

#include "exp_evaluator.h"
#include "syntax_validator.h"

int is_left_assoc(TokenType _operator)
{
	return 1; // every operator is left associative except '^'
}

int precedence(TokenType _operator)
{
	if (_operator == UNARY_MINUS)
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

			// 1.4.2) Also pop the left parenthesis but don’t include it in the output buffer
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
		else if (type == UNARY_MINUS)
		{
			Token* op = NULL;
			st_pop(stack, &op);

			double val = -1 * strtod(op->data, NULL); // changing sign of value

			free(op);
			op = NULL;

			char data[MAX_WORD_LENGTH];
			int len = snprintf(data, 16, "%f", val);
			Token* tk = malloc(sizeof(Token));

			if (tk == NULL)
			{
				status_code = 0;
				break;
			}

			for (int j = 0; j < len; j++)
			{
				tk->data[j] = data[j];
			}
			tk->word_length = len;
			tk->type = NUMBER;
			st_push(stack, tk);
		}
		else if (is_operator(type)) // operator but not unary minus
		{
			Token* op1 = NULL; // operands
			Token* op2 = NULL;
			st_pop(stack, &op1);
			st_pop(stack, &op2);

			double val1 = strtod(op1->data, NULL);
			double val2 = strtod(op2->data, NULL);

			free(op1);
			free(op2);

			op1 = NULL;
			op2 = NULL;

			double partial_result = 0;

			if (type == PLUS)
			{
				partial_result = val1 + val2;
			}
			else if (type == BINARY_MINUS)
			{
				partial_result = val1 - val2;
			}
			else if (type == MUL)
			{
				partial_result = val1 * val2;
			}
			else if (type == DIV)
			{
				if (val2 == 0)
				{
					status_code = 0;
					break;
				}
				partial_result = val1 / val2;
			}

			char data[MAX_WORD_LENGTH];
			int len = snprintf(data, 16, "%f", partial_result);
			Token* tk = malloc(sizeof(Token));

			if (tk == NULL)
			{
				status_code = 0;
				break;
			}
				
			for (int j = 0; j < len; j++)
			{
				tk->data[j] = data[j];
			}
			tk->word_length = len;
			tk->type = NUMBER;
			st_push(stack, tk);
		}

		i++;
	}

	if (stack->items_inside > 0)
	{
		Token* result_token = NULL;
		st_pop(stack, &result_token);
		*result = strtod(result_token->data, NULL);
	}

	free(stack);
	stack = NULL;

	return status_code;
}