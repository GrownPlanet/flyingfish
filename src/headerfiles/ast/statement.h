/*
 * Statement
 *
 * All the needed structs for parsing a statement
 *
 */

#ifndef STATEMENT_H
#define STATEMENT_H

#include "token.h"
#include "expression.h"

/*
 * statement:
 *  - print: print\((expression)\)
 *  - var: var (name) [: type]? = (expression)
 *  - if: if (expression) (statement) [else (statement)]?
 *  - while: while (expression) (statement)
 *  - for: for((expression); (expression); (expression)) (statement)
 *  - block: {(statement)*}
 *
 * TODO: 
 *  - add identifiers to literals!
 *  - add optional types to var's
 */

typedef struct ST_Print_t ST_Print_t;
typedef struct ST_Var_t ST_Var_t;

typedef enum {
    StatementType_Print,
    StatementType_Var,
} StatementType_t;

typedef union {
    ST_Print_t* print;
    ST_Var_t* var;
} StatementValue_t;

typedef struct {
    StatementType_t type;
    StatementValue_t value;
    size_t line;
} Statement_t;

struct ST_Print_t {
    Expression_t* expr;
};

struct ST_Var_t {
    String_t* name;
    Expression_t* expr;
};

void free_statement(Statement_t* expr);

#endif // STATEMENT_H
