// main.c : This file contains the 'main' function. Program execution begins and ends there.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "interfaces/display.h"
#include "interfaces/uart.h"

#include "util/stack.h"

#include "calculator/lexer.h"
#include "calculator/preprocessor.h"
#include "calculator/syntax_validator.h"
#include "calculator/exp_evaluator.h"

// The maximum length of an expression. This will be the size of the expression buffer.
#define MAX_EXP_LEN 64

// Fills a string with whitespaces
void empty_string(char* str, size_t len);

// Evaluates an expression
void process_exp(char* arithm_exp);

// Handles incoming UART messages
void process_input(char msg);

char expression[MAX_EXP_LEN];
int input_index = 0;

int main()
{	
	USART0_init(USART0_def_ubrr());
	
	display_clear();
	display_enable();
	
	while (1)
	{
		char msg = USART0_receive();
		process_input(msg);
	}
}

void process_input(char msg)
{
	char equals = '=';
	char backspace = 0b01111111;
			
	if (msg == equals)
	{
		// Evaluate expression
		process_exp(expression);

		// Start new line in terminal
		USART0_transmit('\n');
				
		// Reset cursor pos in terminal
		for (int i = 0; i < input_index; i++)
		{
			USART0_transmit(backspace);
		}
				
		// Reset input buffer
		empty_string(expression, input_index);
		input_index = 0;
	}
	else if (msg == backspace)
	{
		// Delete previous character
		if (input_index > 0)
		{
			expression[--input_index] = 0x00;
			USART0_transmit(msg);
		}
	}
	else
	{
		// Append character to the expression string and display it in the terminal
		expression[input_index++] = msg;
		USART0_transmit(msg);
	}
}

void process_exp(char* arithm_exp)
{
	display_clear();
	
	Token *tokens[MAX_TOKENS];
    size_t tokens_amount = 0;

    Token *rpn[MAX_TOKENS];
    size_t tokens_amount_rpn = 0;

    int success = lex(arithm_exp, &tokens, MAX_TOKENS, &tokens_amount);
    if (!success) 
	{
		display_write_str("lex error");
		tk_array_free(&tokens, tokens_amount);
		return;
	}

    preprocess_tokens(&tokens, tokens_amount);

    int valid = syntax_validate(&tokens, tokens_amount);
    if (!valid) 
	{
		display_write_str("syntax error");
		tk_array_free(&tokens, tokens_amount);
		return;
	}

    success = shunting_yard(&tokens, tokens_amount, &rpn, MAX_TOKENS, &tokens_amount_rpn);
    if (!success) 
	{
		display_write_str("error in conversion");
		tk_array_free(&tokens, tokens_amount);
		return;
	}

    double result = 0;
    success = evaluate_rpn(&rpn, tokens_amount_rpn, &result);
    if (!success) 
	{
		display_write_str("error in calculation");
		tk_array_free(&tokens, tokens_amount);
		return;
	}
	
	char output[STRLEN_CONST];
	snprintf(output, STRLEN_CONST, "%f", result);
		
	//display_clear();
	display_write_str(output);
    
    tk_array_free(&tokens, tokens_amount);
}

void empty_string(char* str, size_t len)
{
	int i = 0;
	while (i < len)
	{
		*(str + i) = ' ';
		i++;
	}
}
