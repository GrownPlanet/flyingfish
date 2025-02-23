/*
 * Compiler
 *
 * Compile an ast to bytecode
 *
 * */

#ifndef COMPILER_H
#define COMPILER_H

#include "bytecode.h"
#include "statement.h"

ByteCode_t compile(Statement_t* stmt);

#endif // COMPILER_H
