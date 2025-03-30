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
 *  todo:
 *  - block: {(statement)*}
 *  - if: if (expression) (statement) [else (statement)]?
 *  - while: while (expression) (statement)
 *  - for: for((expression); (expression); (expression)) (statement)
 *  done:
 *  - print: print\((expression)\)
 *  - var: var (name) [: type]? = (expression)
 */

typedef struct ST_Print_t ST_Print_t;
typedef struct ST_Var_t ST_Var_t;
typedef struct ST_Assignment_t ST_Assignment_t;
typedef struct ST_Block_t ST_Block_t;

typedef enum {
    StatementType_Print,
    StatementType_Var,
    StatementType_Assignment,
    StatementType_Block,
} StatementType_t;

typedef union {
    ST_Print_t* print;
    ST_Var_t* var;
    ST_Assignment_t* assignment;
    ST_Block_t* block;
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

struct ST_Assignment_t {
    String_t* name;
    Expression_t* expr;
};

struct ST_Block_t {
    Statement_t* stmts;
    size_t len;
};

void free_statement(Statement_t* expr);

#endif // STATEMENT_H
