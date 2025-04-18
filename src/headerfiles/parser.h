/*
 * Parser
 *
 * Parse the list of tokens into a tree like structure.
 *
 */

#ifndef PARSER_H
#define PARSER_H

#include "ast/statement.h"
#include "scanner.h"
#include "parser.h"

typedef struct {
    Statement_t* statements;
    size_t len;
    bool had_error;
} ParseResult_t;

ParseResult_t parse(ScanResult_t tokens);

#endif // PARSER_H
