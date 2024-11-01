/*
 * Token
 *
 * All tokens needed for the programming language
 * */

#pragma once

#include <stdbool.h>

typedef enum {
    // parens, braces (0 - 3)
    LeftParen, RightParen, LeftBrace, RightBrace, 
    // (primarely) math (4 - 13)
    Minus, Plus, Slash, Star, EqualEqual, Greater, GreaterEqual, Lesser, LesserEqual, BangEqual, 
    // boolean (14 - 16)
    Bang, And, Or,
    // other (17 - 20)
    Comma, Point, Semicolon, Equal,
    // variables (21 - 26)
    Identifier, IntV, FloatV, CharV, StringV, BoolV,
    // types (27 - 31)
    IntT, FloatT, CharT, StringT, BoolT,
    // keywoards (32 - 37)
    If, Else, Func, For, Return, While,
} TokenType_t;

typedef union {
    double db;
    long ln;
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
