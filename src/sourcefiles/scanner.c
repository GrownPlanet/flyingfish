#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"

char peek_char(char* input, size_t input_len, size_t index);
void push_token(Token_t** tokens, size_t* tokens_capacity, size_t* tokens_len, Token_t token);
char* extract_num(char* input, size_t input_len, size_t* index, bool* is_float);

ScanResult_t scan(char* input, size_t input_len) {
    Token_t* tokens = (Token_t*)malloc(sizeof(Token_t));

    size_t tokens_capacity = 1;
    size_t tokens_len = 0;

    for (size_t i = 0; i < input_len; i++) {
        char ch = input[i];
        switch (ch) {
            case '(':
                push_token(&tokens, &tokens_len, &tokens_capacity, new_token(LeftParen, NULL));
                break;
            case ')':
                push_token(&tokens, &tokens_len, &tokens_capacity, new_token(RightParen, NULL));
                break;
            case '{':
                push_token(&tokens, &tokens_len, &tokens_capacity, new_token(LeftBrace, NULL));
                break;
            case '}':
                push_token(&tokens, &tokens_len, &tokens_capacity, new_token(RightBrace, NULL));
                break;
            case '+':
                push_token(&tokens, &tokens_len, &tokens_capacity, new_token(Plus, NULL));
                break;
            case '-':
                push_token(&tokens, &tokens_len, &tokens_capacity, new_token(Minus, NULL));
                break;
            case '*':
                push_token(&tokens, &tokens_len, &tokens_capacity, new_token(Star, NULL));
                break;
            case '/':
                if (peek_char(input, input_len, i) == '/') {
                    while (ch != '\n') {
                        i++;
                    }
                } else {
                    push_token(&tokens, &tokens_len, &tokens_capacity, new_token(Slash, NULL));
                }
                break;
            case '=':
                if (peek_char(input, input_len, i) == '=') {
                    push_token(&tokens, &tokens_len, &tokens_capacity, new_token(EqualEqual, NULL));
                    i++;
                } else {
                    push_token(&tokens, &tokens_len, &tokens_capacity, new_token(Equal, NULL));
                }
                break;
            case '>':
                if (peek_char(input, input_len, i) == '=') {
                    push_token(&tokens, &tokens_len, &tokens_capacity, new_token(GreaterEqual, NULL));
                    i++;
                } else {
                    push_token(&tokens, &tokens_len, &tokens_capacity, new_token(Greater, NULL));
                }
                break;
            case '<':
                if (peek_char(input, input_len, i) == '=') {
                    push_token(&tokens, &tokens_len, &tokens_capacity, new_token(LesserEqual, NULL));
                    i++;
                } else {
                    push_token(&tokens, &tokens_len, &tokens_capacity, new_token(Lesser, NULL));
                }
                break;
            case '&':
                push_token(&tokens, &tokens_len, &tokens_capacity, new_token(Bang, NULL));
                break;
            case '|':
                push_token(&tokens, &tokens_len, &tokens_capacity, new_token(Or, NULL));
                break;
            case ',':
                push_token(&tokens, &tokens_len, &tokens_capacity, new_token(Comma, NULL));
                break;
            case '.':
                push_token(&tokens, &tokens_len, &tokens_capacity, new_token(Point, NULL));
                break;
            case ';':
                push_token(&tokens, &tokens_len, &tokens_capacity, new_token(Semicolon, NULL));
                break;
            case '\n':
            case '\r':
            case ' ':
                break;
            default:
                // check if c is numeric
                if ('0' <= ch && ch <= '9') {
                    bool is_float = false;
                    char* num_str = extract_num(input, input_len, &i, &is_float);
                    break;
                }

                push_token(&tokens, &tokens_len, &tokens_capacity, new_token(-1, NULL));
                break;
        }
    }
    /*
    Int, Float, Char, String, Bool,
    Identifier, If, Else, Func, For, Return, While,
    */

    ScanResult_t scan_result = {
        tokens, tokens_len
    };

    return scan_result;
}

char peek_char(char* input, size_t input_len, size_t index) {
    if (index >= input_len) {
        return input[index];
    }

    return input[index + 1];
}

void push_token(Token_t** tokens, size_t* tokens_len, size_t* tokens_capacity, Token_t token) {
    if (*tokens_len >= *tokens_capacity) {
        *tokens_capacity *= 2;
        *tokens = (Token_t*)realloc(*tokens, *tokens_capacity * sizeof(Token_t));

        if (tokens == NULL) {
            printf("Realloc failed!\n");
            exit(1);
        }
    }

    (*tokens)[*tokens_len] = token;
    (*tokens_len)++;
}

char* extract_num(char* input, size_t input_len, size_t* index, bool* is_float) {
    char* num = (char*)malloc(2 * sizeof(char));
    size_t num_capacity = 1;
    size_t num_len = 0;

    while ('0' <= input[*index] && input[*index] <= '9') {
        num[num_len] = input[*index];
        *index++;
    }

    if (input[*index] == '.') {
        *is_float = true;
        while ('0' <= input[*index] && input[*index] <= '9') {
            num[num_len] = input[*index];
            *index++;
        }
    }

    return num;
}
