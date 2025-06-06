/*
 * Emitter
 *
 * Emit the compiled program
 *
 * */

#ifndef EMITTER_H
#define EMITTER_H

#include "bytecode.h"

int emit(ByteCode_t* bytecode, char* filename);

#endif // EMITTER_H
