#include <stddef.h>
#include <stdio.h>

#include "token.h"

Token_t new_token(TokenType_t type, size_t line, Literal_t* literal) {
    Token_t token = {
        .type = type,
        .line = line,
        .literal = literal,
    };

    return token;
}

void print_token_type(TokenType_t token_type) {
    switch (token_type) {
        case TokenType_LeftParen: printf("LeftParen"); break;
        case TokenType_RightParen: printf("RightParen"); break;
        case TokenType_LeftBrace: printf("LeftBrace"); break;
        case TokenType_RightBrace: printf("RightBrace"); break;
        case TokenType_Minus: printf("Minus"); break;
        case TokenType_Plus: printf("Plus"); break;
        case TokenType_Slash: printf("Slash"); break;
        case TokenType_Star: printf("Star"); break;
        case TokenType_EqualEqual: printf("EqualEqual"); break;
        case TokenType_Greater: printf("Greater"); break;
        case TokenType_GreaterEqual: printf("GreaterEqual"); break;
        case TokenType_Lesser: printf("Lesser"); break;
        case TokenType_LesserEqual: printf("LesserEqual"); break;
        case TokenType_BangEqual: printf("BangEqual"); break;
        case TokenType_Bang: printf("Bang"); break;
        case TokenType_And: printf("And"); break;
        case TokenType_Or: printf("Or"); break;
        case TokenType_Comma: printf("Comma"); break;
        case TokenType_Point: printf("Point"); break;
        case TokenType_Semicolon: printf("Semicolon"); break;
        case TokenType_Equal: printf("Equal"); break;
        case TokenType_Identifier: printf("Identifier"); break;
        case TokenType_IntV: printf("IntV"); break;
        case TokenType_FloatV: printf("FloatV"); break;
        case TokenType_CharV: printf("CharV"); break;
        case TokenType_StringV: printf("StringV"); break;
        case TokenType_BoolV: printf("BoolV"); break;
        case TokenType_IntT: printf("IntT"); break;
        case TokenType_FloatT: printf("FloatT"); break;
        case TokenType_CharT: printf("CharT"); break;
        case TokenType_StringT: printf("StringT"); break;
        case TokenType_BoolT: printf("BoolT"); break;
        case TokenType_If: printf("If"); break;
        case TokenType_Else: printf("Else"); break;
        case TokenType_Func: printf("Func"); break;
        case TokenType_For: printf("For"); break;
        case TokenType_Return: printf("Return"); break;
        case TokenType_While: printf("While"); break;
        case TokenType_Print: printf("Print"); break;
        case TokenType_Var: printf("Var"); break;
        default: printf("ERR: %d", token_type);
    }
}
