#pragma once

#include "../util/tokens.h"

int is_operator(TokenType symbol);
int precedence(TokenType _operator);
int is_left_assoc(TokenType _operator);

// Converts infix expression into RPN expression.
// Returns status code.
// tokens -> tokens in infix notation
// tokens_size -> amount of input tokens
// rpn_out -> RPN expression (output)
// output_max_size -> max allocated size of 'rpn_out'
// output_actual_size -> the used size of 'rpn_out' (output)
int shunting_yard(const Token** tokens, size_t tokens_size, Token** rpn_out, size_t output_max_size, size_t* output_actual_size);

// Evaluates RPN expression.
// Returns status code
int evaluate_rpn(const Token** rpn, size_t size, double* result);