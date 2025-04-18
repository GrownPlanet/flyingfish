/*
 * Compiler
 *
 * Compile an ast to bytecode
 *
 * */

#ifndef COMPILER_H
#define COMPILER_H

#include "bytecode.h"
#include "ast/statement.h"
#include "parser.h"

ByteCode_t compile(ParseResult_t  parse_result);

#endif // COMPILER_H
