/*
 * Token
 *
 * All tokens needed for the programming language.
 *
 * */

#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum {
    // parens, braces (0 - 3)
    TokenType_LeftParen, TokenType_RightParen, TokenType_LeftBrace, TokenType_RightBrace, 
    // (primarely) math (4 - 13)
    TokenType_Minus, TokenType_Plus, TokenType_Slash, TokenType_Star, TokenType_EqualEqual,
    TokenType_Greater, TokenType_GreaterEqual, TokenType_Lesser, TokenType_LesserEqual, 
    TokenType_BangEqual, 
    // boolean (14 - 16)
    TokenType_Bang, TokenType_And, TokenType_Or,
    // other (17 - 20)
    TokenType_Comma, TokenType_Point, TokenType_Semicolon, TokenType_Equal,
    // variables (21 - 26)
    TokenType_Identifier, TokenType_IntV, TokenType_FloatV, TokenType_CharV, TokenType_StringV,
    TokenType_BoolV,
    // types (27 - 31)
    TokenType_IntT, TokenType_FloatT, TokenType_CharT, TokenType_StringT, TokenType_BoolT,
    // keywoards (32 - 37)
    TokenType_If, TokenType_Else, TokenType_Func, TokenType_For, TokenType_Return, TokenType_While,
} TokenType_t;

typedef union {
    int64_t ln;
    double db;
    char* str;
    char ch;
    bool b;
} Literal_t;

typedef struct {
    TokenType_t type;
    size_t line;
    Literal_t* literal;
} Token_t;

Token_t new_token(TokenType_t type, size_t line, Literal_t* literal);
void print_token_type(TokenType_t token_type);

#endif // TOKEN_H
