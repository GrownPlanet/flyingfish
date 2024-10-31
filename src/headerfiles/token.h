/*
 * Token
 *
 * All tokens needed for the programming language
 * */

#pragma once

typedef enum {
    // parens, braces
    LeftParen, RightParen, LeftBrace, RightBrace, 
    // (primarely) math
    Minus, Plus, Slash, Star, EqualEqual, Greater, GreaterEqual, Lesser, LesserEqual, BangEqual, 
    // boolean
    Bang, And, Or,
    // other
    Comma, Point, Semicolon, Equal,
    // Variables
    Identifier, Int, Float, Char, String, Bool,
    // keywoards
    If, Else, Func, For, Return, While,
} TokenType_t;

typedef union {
    float fl;
    int in;
    char* str;
} Literal_t;

typedef struct {
    TokenType_t type;
    Literal_t* literal;
} Token_t;

Token_t new_token(TokenType_t type, Literal_t* literal);
