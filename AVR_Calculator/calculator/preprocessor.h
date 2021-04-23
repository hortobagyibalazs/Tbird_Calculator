#pragma once

#include "../util/tokens.h"
#include "constants.h"

// Mostly for replacing constants/variables with numbers
// Available constants/variables: PI, e, Ans
void preprocess_tokens(Token** tokens, size_t size);