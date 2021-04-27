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

int main()
{
	display_enable();
	display_clear();

    Token *tokens[MAX_TOKENS];
    size_t tokens_amount = 0;

    Token *rpn[MAX_TOKENS];
    size_t tokens_amount_rpn = 0;

    char* exp = "sqrt(-3600+13600)*(2^(-2)+sin(30*pi/180))"; // supposed to be 75

    int success = lex(exp, &tokens, MAX_TOKENS, &tokens_amount);
    if (!success) display_write("lex error");

    preprocess_tokens(&tokens, tokens_amount);

    int valid = syntax_validate(&tokens, tokens_amount);
    if (!valid) display_write("syntax error");

    success = shunting_yard(&tokens, tokens_amount, &rpn, MAX_TOKENS, &tokens_amount_rpn);
    if (!success) display_write("error in conversion");

    double result = 0;
    success = evaluate_rpn(&rpn, tokens_amount_rpn, &result);
    if (!success) display_write("error in calculation");
	
	char output[STRLEN_CONST];
	snprintf(output, STRLEN_CONST, "%f", result);
		
	display_write(output);
    
    tk_array_free(&tokens, tokens_amount);
}
