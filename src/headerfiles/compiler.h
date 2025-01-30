/*
 * Compiler
 *
 * Compile an ast to bytecode
 *
 * */

// TODO:
// - add flags for type, addressing mode instead of 4 byte numbers for each :/

#ifndef COMPILER_H
#define COMPILER_H

#include "bytecode.h"
#include "expression.h"

ByteCode_t compile(Expression_t* expr);

#endif // COMPILER_H
