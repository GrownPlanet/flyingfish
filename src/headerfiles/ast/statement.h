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
 *  - print: "print" ( expression )
 *  - var: "var" name = expression
 *  - if: "if" expression block [else [if | block]]?
 *  - while: "while" expression block
 *  - for "for" ( expression; expression; expression ) block
 *
 *  - block: { statement* }
 */

typedef struct ST_Print_t ST_Print_t;
typedef struct ST_Var_t ST_Var_t;

typedef enum {
    StatementType_Print,
} StatementType_t;

typedef union {
    ST_Print_t* print;
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
    Expression_t* expr;
};

void free_statement(Statement_t* expr);

#endif // STATEMENT_H
