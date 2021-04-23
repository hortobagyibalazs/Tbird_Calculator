#pragma once

#include "../util/tokens.h"

// Context-sensitive lexical analysis
// Returns success code (1 for success, 0 for failure)
int lex(const char* exp, Token** tokens_out, size_t tokens_max_size, size_t* tokens_actual_size);