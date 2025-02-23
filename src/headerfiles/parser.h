/*
 * Parser
 *
 * Parse the list of tokens into a tree like structure.
 *
 */

#ifndef PARSER_H
#define PARSER_H

#include "statement.h"
#include "scanner.h"

Statement_t* parse(ScanResult_t tokens);

#endif // PARSER_H
