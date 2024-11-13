/*
 * Parser
 *
 * Parse the list of tokens into a tree like structure.
 *
 */

#ifndef PARSER_H
#define PARSER_H

#include "expression.h"

Expression_t* parse(Token_t* tokens, size_t len);

#endif // PARSER_H
