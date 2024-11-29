/*
 * Compiler
 *
 * Compile an ast to bytecode
 *
 * */

#ifndef COMPILER_H
#define COMPILER_H

#include "bytecode.h"
#include "expression.h"

ByteCode_t compile(Expression_t* expr);

#endif // COMPILER_H
