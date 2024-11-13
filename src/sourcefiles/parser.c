#include "parser.h"
#include "expression.h"
#include "token.h"
#include "error.h"

/*
 * | primary
 * |
 * | unary
 * |
 * | multiplication/division
 * |
 * | plus/minus
 * | 
 * | comparison
 * | 
 * | equalequal/notequal
 * |
 * | and/or
 * V
 * */

// void advance(size_t len, size_t* index);

Expression_t* parse_primary(Token_t* tokens, size_t len, size_t* index) {
    Token_t token = tokens[*index];

    Expression_t* expr = (Expression_t*)malloc(sizeof(Expression_t));

    switch (token.type) {
        case TokenType_IntV:
        case TokenType_FloatV:
        case TokenType_CharV:
        case TokenType_StringV:
        case TokenType_BoolV:
            expr->type = ExpressionType_Literal;

            EV_Literal_t* lit = (EV_Literal_t*)malloc(sizeof(EV_Literal_t));
            lit->type = token.type;
            lit->value = token.literal;

            ExpressionValue_t v;
            v.literal = lit;
            expr->value = v;
            break;
        case TokenType_LeftParen:
            // TODO: grouping
            report_error("TODO: parser.c line 47", token.line);
            return NULL;
        default:
            report_error("expected primary", token.line);
            return NULL;
    }

    return expr;
}

Expression_t* parse(Token_t* tokens, size_t len) {
    size_t index = 0;
   return parse_primary(tokens, len, &index);
}

void advance(size_t len, size_t* index) {
    if (*index < len) {
        (*index)++;
    }
}
