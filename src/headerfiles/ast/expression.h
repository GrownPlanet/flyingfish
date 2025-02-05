/*
 * Expression
 *
 * A collection of values and other expressions that can be parsed to a single value.
 *
 */

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "token.h"

typedef struct EV_Binary_t EV_Binary_t;
typedef struct EV_Unary_t EV_Unary_t;
typedef struct EV_Literal_t EV_Literal_t;

typedef enum {
    ExpressionType_Binary,
    ExpressionType_Unary,
    ExpressionType_Literal,
} ExpressionType_t;

typedef union {
    EV_Binary_t* binary;
    EV_Unary_t* unary;
    EV_Literal_t* literal;
} ExpressionValue_t;

typedef struct {
    ExpressionType_t type;
    ExpressionValue_t value;
    size_t line;
} Expression_t;

struct EV_Binary_t {
    Expression_t left;
    Token_t operator;
    Expression_t right;
    TokenType_t type;
};

struct EV_Unary_t {
    TokenType_t operator;
    Expression_t operant;
    TokenType_t type;
};

struct EV_Literal_t {
    TokenType_t type;
    Literal_t* value;
};

void free_expression(Expression_t* expr);
TokenType_t get_expression_type(Expression_t* expr);

#endif // EXPRESSION_H
