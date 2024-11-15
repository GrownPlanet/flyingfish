/*
 * Parser
 *
 * Parse the list of tokens into a tree like structure.
 *
 */

#ifndef PARSER_H
#define PARSER_H

#include "expression.h"
#include "scanner.h"

Expression_t* parse(ScanResult_t tokens);

#endif // PARSER_H
