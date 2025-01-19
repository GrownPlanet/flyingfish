#include <stdio.h>

#include "parser.h"
#include "expression.h"
#include "token.h"
#include "error.h"

/*
 * | primary V
 * |
 * | unary V
 * |
 * | multiplication/division V
 * |
 * | plus/minus V
 * | 
 * | // doing the rest later, I want something that can compile the code :)
 * | comparison
 * | 
 * | equalequal/notequal
 * |
 * | and/or
 * V
 * */

void advance(size_t len, size_t* index);
Expression_t* parse(ScanResult_t tokens);
Expression_t* parse_term(Token_t* tokens, size_t len, size_t* index);

Expression_t* parse_primary(Token_t* tokens, size_t len, size_t* index) {
    Token_t token = tokens[*index];

    Expression_t* expr = (Expression_t*)malloc(sizeof(Expression_t));
    if (expr == NULL) {
        report_error("malloc failed!\n", token.line);
        return NULL;
    }
    expr->line = token.line;

    switch (token.type) {
        case TokenType_IntV:
        case TokenType_FloatV:
        case TokenType_CharV:
        case TokenType_StringV:
        case TokenType_BoolV:
            expr->type = ExpressionType_Literal;

            EV_Literal_t* lit = (EV_Literal_t*)malloc(sizeof(EV_Literal_t));
            if (lit == NULL) {
                report_error("malloc failed!\n", token.line);
                return NULL;
            }

            lit->type = token.type;
            lit->value = token.literal;

            ExpressionValue_t v;
            v.literal = lit;
            expr->value = v;
            advance(len, index);
            break;
        case TokenType_LeftParen:
            advance(len, index);
            Expression_t* expr = parse_term(tokens, len, index);

            token = tokens[*index];

            if (token.type != TokenType_RightParen) {
                report_error("expected left paren after expression", token.line);
                return NULL;
            }
            advance(len, index);

            return expr;
        default:
            report_error("expected primary", token.line);
            return NULL;
    }

    return expr;
}

Expression_t* parse_unary(Token_t* tokens, size_t len, size_t* index) {
    Token_t token = tokens[*index];

    if (token.type == TokenType_Bang || token.type == TokenType_Minus) {
        advance(len, index);

        Expression_t* expr = (Expression_t*)malloc(sizeof(Expression_t));
        if (expr == NULL) {
            report_error("malloc failed!\n", token.line);
            return NULL;
        }
        expr->line = token.line;

        expr->type = ExpressionType_Unary;

        EV_Unary_t* un = (EV_Unary_t*)malloc(sizeof(EV_Unary_t));
        if (un == NULL) {
            report_error("malloc failed!\n", token.line);
            return NULL;
        }
        un->operator = token.type;

        Expression_t* operant = parse_primary(tokens, len, index);
        if (operant == NULL) {
            return NULL;
        }
        un->operant = *operant;

        ExpressionValue_t v;
        v.unary = un;
        expr->value = v;

        return expr;
    }

    return parse_primary(tokens, len, index);
}

Expression_t* parse_binary(
    Token_t* tokens, size_t len, size_t* index, TokenType_t types[2], 
    Expression_t* (*base_func)(Token_t*, size_t, size_t*) // higher order function
) {
    Expression_t* expr = base_func(tokens, len, index);

    Token_t token = tokens[*index];
    while (token.type == types[0] || token.type == types[1]) {
        Expression_t* right = (Expression_t*)malloc(sizeof(Expression_t));
        if (right == NULL) {
            report_error("malloc failed!\n", token.line);
            return NULL;
        }
        right->line = token.line;

        right->type = ExpressionType_Binary;

        EV_Binary_t* bin = (EV_Binary_t*)malloc(sizeof(EV_Binary_t));
        if (bin == NULL) {
            report_error("malloc failed!\n", token.line);
            return NULL;
        }

        bin->left = *expr;
        bin->operator = token;

        advance(len, index);
        token = tokens[*index];

        bin->right = *base_func(tokens, len, index);
        token = tokens[*index];

        ExpressionValue_t v;
        v.binary = bin;
        right->value = v;

        expr = right;
    }

    return expr;

}

Expression_t* parse_factor(Token_t* tokens, size_t len, size_t* index) {
    TokenType_t types[2] = {TokenType_Star, TokenType_Slash};
    return parse_binary(tokens, len, index, types, parse_unary);
}

Expression_t* parse_term(Token_t* tokens, size_t len, size_t* index) {
    TokenType_t types[2] = {TokenType_Plus, TokenType_Minus};
    return parse_binary(tokens, len, index, types, parse_factor);
}

Expression_t* parse(ScanResult_t tokens) {
    size_t index = 0;
    return parse_term(tokens.tokens, tokens.len, &index);
}

void advance(size_t len, size_t* index) {
    if (*index < len) {
        (*index)++;
    }
}
