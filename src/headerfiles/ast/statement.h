/*
 * Statement
 *
 * All the needed structs for parsing a statement
 *
 */

#ifndef STATEMENT_H
#define STATEMENT_H

#include "token.h"
#include "ast/expression.h"

/*
 * statement:
 *  todo:
 *  - while: while (expression) (statement)
 *  - for: for((expression); (expression); (expression)) (statement)
 *  done:
 *  - if: if (expression) (statement) [else (statement)]?
 *  - block: {(statement)*}
 *  - print: print\((expression)\)
 *  - var: var (name) [: type]? = (expression)
 */

typedef struct ST_Print_t ST_Print_t;
typedef struct ST_Var_t ST_Var_t;
typedef struct ST_Assignment_t ST_Assignment_t;
typedef struct ST_Block_t ST_Block_t;
typedef struct ST_If_t ST_If_t;
typedef struct ST_While_t ST_While_t;

typedef enum {
    StatementType_Print,
    StatementType_Var,
    StatementType_Assignment,
    StatementType_Block,
    StatementType_If,
    StatementType_While,
} StatementType_t;

typedef union {
    ST_Print_t* print;
    ST_Var_t* var;
    ST_Assignment_t* assignment;
    ST_Block_t* block;
    ST_If_t* if_s;
    ST_While_t* while_s;
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
    TokenType_t type;
};

struct ST_Assignment_t {
    String_t* name;
    Expression_t* expr;
};

struct ST_Block_t {
    Statement_t* stmts;
    size_t len;
};

struct ST_If_t {
    Expression_t* condition;
    Statement_t* if_body;
    Statement_t* else_body; // NULL if none
};

struct ST_While_t {
    Expression_t* condition;
    Statement_t* body;
};

void free_statement(Statement_t* expr);

#endif // STATEMENT_H
