#pragma once

#include "../util/tokens.h"

// Returns 1 if valid, 0 when exp is invalid
int syntax_validate(const Token** tokens, size_t size);