#include <stdio.h>

#include "numtypes.h"
#include "parser.h"
#include "expression.h"
#include "statement.h"
#include "token.h"

typedef struct {
    Token_t* tokens;
    size_t len;
    size_t index;
} Parser_t;

void advance(Parser_t* parser);
Expression_t* parse(ScanResult_t tokens);
Expression_t* parse_term(Parser_t* parser);

Expression_t* parse_primary(Parser_t* parser) {
    // literal
    Token_t token = parser->tokens[parser->index];

    Expression_t* expr = (Expression_t*)malloc(sizeof(Expression_t));
    if (expr == NULL) {
        printf("Malloc failed!\n");
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
                printf("Malloc failed!\n");
                return NULL;
            }

            lit->type = token.type;
            lit->value = token.literal;

            ExpressionValue_t v;
            v.literal = lit;
            expr->value = v;
            advance(parser);
            break;
        case TokenType_LeftParen:
            advance(parser);
            Expression_t* expr = parse_term(parser);

            token = parser->tokens[parser->index];

            if (token.type != TokenType_RightParen) {
                printf("Expected left paren after expression on line %" PRIu "\n", token.line);
                return NULL;
            }
            advance(parser);

            return expr;
        default:
            printf("Expected primary on line %" PRIu "\n", token.line);
            return NULL;
    }

    return expr;
}

Expression_t* parse_unary(Parser_t* parser) {
    // -[primary]
    Token_t token = parser->tokens[parser->index];

    if (token.type == TokenType_Bang || token.type == TokenType_Minus) {
        advance(parser);

        Expression_t* expr = (Expression_t*)malloc(sizeof(Expression_t));
        if (expr == NULL) {
            printf("Malloc failed!\n");
            return NULL;
        }
        expr->line = token.line;

        expr->type = ExpressionType_Unary;

        EV_Unary_t* un = (EV_Unary_t*)malloc(sizeof(EV_Unary_t));
        if (un == NULL) {
            printf("Malloc failed!\n");
            return NULL;
        }
        un->operator = token.type;

        Expression_t* operant = parse_primary(parser);
        if (operant == NULL) {
            return NULL;
        }
        un->operant = *operant;

        un->type = get_expression_type(operant);

        ExpressionValue_t v;
        v.unary = un;
        expr->value = v;

        return expr;
    }

    return parse_primary(parser);
}

Expression_t* parse_binary(
    Parser_t* parser, TokenType_t* types, size_t types_len,
    Expression_t* (*base_func)(Parser_t* parser) // higher order function
) {
    Expression_t* expr = base_func(parser);

    Token_t token = parser->tokens[parser->index];
    bool running = false;
    for (size_t i = 0; i < types_len; i++) {
        running |= token.type == types[i];
    }
    while (running) {
        Expression_t* right = (Expression_t*)malloc(sizeof(Expression_t));
        if (right == NULL) {
            printf("Malloc failed!\n");
            return NULL;
        }
        right->line = token.line;

        right->type = ExpressionType_Binary;

        EV_Binary_t* bin = (EV_Binary_t*)malloc(sizeof(EV_Binary_t));
        if (bin == NULL) {
            printf("Malloc failed!\n");
            return NULL;
        }

        bin->left = *expr;
        bin->operator = token;

        advance(parser);
        token = parser->tokens[parser->index];

        bin->right = *base_func(parser);
        token = parser->tokens[parser->index];

        TokenType_t t1 = get_expression_type(expr);
        TokenType_t t2 = get_expression_type(&bin->right);
        if (t1 != t2) {
            printf("Types do operations with ");
            print_token_type(t1);
            printf(" and ");
            print_token_type(t2);
            printf("on line %" PRIu "\n", token.line);
            return NULL;
        }
        bin->type = t1;

        ExpressionValue_t v;
        v.binary = bin;
        right->value = v;

        expr = right;

        running = false;
        for (size_t i = 0; i < types_len; i++) {
            running |= token.type == types[i];
        }
    }

    return expr;

}

Expression_t* parse_factor(Parser_t* parser) {
    // * or /
    size_t types_len = 2;
    TokenType_t* types = (TokenType_t*)malloc(sizeof(TokenType_t) * types_len );
    if (types == NULL) {
        printf("Malloc failed!\n");
        return NULL;
    }
    types[0] = TokenType_Star;
    types[1] = TokenType_Slash;

    return parse_binary(parser, types, types_len, parse_unary);
}

Expression_t* parse_term(Parser_t* parser) {
    // + or -
    size_t types_len = 2;
    TokenType_t* types = (TokenType_t*)malloc(sizeof(TokenType_t) * types_len);
    if (types == NULL) {
        printf("Malloc failed!\n");
        return NULL;
    }
    types[0] = TokenType_Plus;
    types[1] = TokenType_Minus;
    return parse_binary(parser, types, types_len, parse_factor);
}

Expression_t* parse_comparison(Parser_t* parser) {
    // > or >= or < or <=
    size_t types_len = 4;
    TokenType_t* types = (TokenType_t*)malloc(sizeof(TokenType_t) * types_len);
    if (types == NULL) {
        printf("Malloc failed!\n");
        return NULL;
    }
    types[0] = TokenType_Greater;
    types[1] = TokenType_GreaterEqual;
    types[2] = TokenType_Lesser;
    types[3] = TokenType_LesserEqual;
    return parse_binary(parser, types, types_len, parse_term);
}

Expression_t* parse_eq_neq(Parser_t* parser) {
    // == or !=
    size_t types_len = 2;
    TokenType_t* types = (TokenType_t*)malloc(sizeof(TokenType_t) * types_len);
    if (types == NULL) {
        printf("Malloc failed!\n");
        return NULL;
    }
    types[0] = TokenType_EqualEqual;
    types[1] = TokenType_BangEqual;
    return parse_binary(parser, types, types_len, parse_comparison);
}

Expression_t* parse_expr(Parser_t* parser) {
    // && or ||
    size_t types_len = 2;
    TokenType_t* types = (TokenType_t*)malloc(sizeof(TokenType_t) * types_len);
    if (types == NULL) {
        printf("Malloc failed!\n");
        return NULL;
    }
    types[0] = TokenType_And;
    types[1] = TokenType_Or;
    return parse_binary(parser, types, types_len, parse_eq_neq);
}

Statement_t* parse_print() {}

Statement_t* parse_statement(Parser_t* parser) {
    Token_t token = parser->tokens[parser->index];
    switch (token) {
        case TokenType_Print:
            return parse_print(parser);
        default: 
            printf("Statement %d implemented yet!\n", token);
            return NULL;
    }
}

Expression_t* parse(ScanResult_t tokens) {
    Parser_t parser = {
        .tokens = tokens.tokens,
        .len = tokens.len,
        .index = 0,
    };

    return parse_expr(&parser);
}

void advance(Parser_t* parser) {
    if (parser->index < parser->len) {
        (parser->index)++;
    }
}
