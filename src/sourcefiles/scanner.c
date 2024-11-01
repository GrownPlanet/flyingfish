#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array_utils.h"
#include "scanner.h"

char peek_char(char* input, size_t input_len, size_t index);
void push_token(Token_t** tokens, size_t* t_capacity, size_t* t_len, Token_t token);

Token_t keywoard_to_token(char* identifier);

char* extract_identifier(char* input, size_t input_len, size_t* index);
char* extract_num(char* input, size_t input_len, size_t* index, bool* is_float);
char* extract_string(char* input, size_t input_len, size_t* index);

ScanResult_t scan(char* input, size_t input_len) {
    Token_t* tokens = (Token_t*)malloc(sizeof(Token_t));
    if (tokens == NULL) {
        printf("malloc failed!\n");
        exit(1);
    }

    size_t t_capacity = 1;
    size_t t_len = 0;

    for (size_t i = 0; i < input_len; i++) {
        char ch = input[i];
        switch (ch) {
            case '(':
                push_token(&tokens, &t_len, &t_capacity, new_token(LeftParen, NULL));
                break;
            case ')':
                push_token(&tokens, &t_len, &t_capacity, new_token(RightParen, NULL));
                break;
            case '{':
                push_token(&tokens, &t_len, &t_capacity, new_token(LeftBrace, NULL));
                break;
            case '}':
                push_token(&tokens, &t_len, &t_capacity, new_token(RightBrace, NULL));
                break;
            case '+':
                push_token(&tokens, &t_len, &t_capacity, new_token(Plus, NULL));
                break;
            case '-':
                push_token(&tokens, &t_len, &t_capacity, new_token(Minus, NULL));
                break;
            case '*':
                push_token(&tokens, &t_len, &t_capacity, new_token(Star, NULL));
                break;
            case '/':
                if (peek_char(input, input_len, i) == '/') {
                    while (input[i] != '\n') {
                        i++;
                    }
                } else {
                    push_token(&tokens, &t_len, &t_capacity, new_token(Slash, NULL));
                }
                break;
            case '=':
                if (peek_char(input, input_len, i) == '=') {
                    push_token(&tokens, &t_len, &t_capacity, new_token(EqualEqual, NULL));
                    i++;
                } else {
                    push_token(&tokens, &t_len, &t_capacity, new_token(Equal, NULL));
                }
                break;
            case '>':
                if (peek_char(input, input_len, i) == '=') {
                    push_token(&tokens, &t_len, &t_capacity, new_token(GreaterEqual, NULL));
                    i++;
                } else {
                    push_token(&tokens, &t_len, &t_capacity, new_token(Greater, NULL));
                }
                break;
            case '<':
                if (peek_char(input, input_len, i) == '=') {
                    push_token(&tokens, &t_len, &t_capacity, new_token(LesserEqual, NULL));
                    i++;
                } else {
                    push_token(&tokens, &t_len, &t_capacity, new_token(Lesser, NULL));
                }
                break;
            case '&':
                push_token(&tokens, &t_len, &t_capacity, new_token(Bang, NULL));
                break;
            case '|':
                push_token(&tokens, &t_len, &t_capacity, new_token(Or, NULL));
                break;
            case ',':
                push_token(&tokens, &t_len, &t_capacity, new_token(Comma, NULL));
                break;
            case '.':
                push_token(&tokens, &t_len, &t_capacity, new_token(Point, NULL));
                break;
            case ';':
                push_token(&tokens, &t_len, &t_capacity, new_token(Semicolon, NULL));
                break;
            case '"': {
                char* str = extract_string(input, input_len, &i);
                Literal_t* lit = (Literal_t*)malloc(sizeof(Literal_t));
                if (lit == NULL) {
                    printf("malloc failed!\n");
                    exit(1);
                }

                lit->str = str;
                push_token(&tokens, &t_len, &t_capacity, new_token(String, lit));
                break;
            }
            case '\'': {
                i++;
                Literal_t* lit = (Literal_t*)malloc(sizeof(Literal_t));
                if (lit == NULL) {
                    printf("malloc failed!\n");
                    exit(1);
                }

                lit->ch = input[i];
                push_token(&tokens, &t_len, &t_capacity, new_token(Char, lit));

                i++;
                if (input[i] != '\'') {
                    printf("expected `'` after char!\n");
                    exit(1);
                }

                break;
            }
            case '\n':
            case '\r':
            case ' ':
                break;
            default:
                // check if c is numeric
                if ('0' <= ch && ch <= '9') {
                    bool is_float = false;
                    char* num_str = extract_num(input, input_len, &i, &is_float);

                    Literal_t* lit = (Literal_t*)malloc(sizeof(Literal_t));
                    if (lit == NULL) {
                        printf("malloc failed!\n");
                        exit(1);
                    }

                    if (is_float) {
                        lit->db = strtod(num_str, NULL);
                        push_token(&tokens, &t_len, &t_capacity, new_token(Float, lit));
                    } 
                    else {
                        lit->ln = atol(num_str);
                        push_token(&tokens, &t_len, &t_capacity, new_token(Int, lit));
                    }

                    i--;

                    break;
                }

                // check if c is is alphabetic
                if (('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z')) {
                    char* identifier = extract_identifier(input, input_len, &i);
                    push_token(&tokens, &t_len, &t_capacity, keywoard_to_token(identifier));

                    i--;

                    break;
                }

                push_token(&tokens, &t_len, &t_capacity, new_token(-1, NULL));
                break;
        }
    }

    ScanResult_t scan_result = {
        tokens, t_len
    };

    return scan_result;
}

char peek_char(char* input, size_t input_len, size_t index) {
    if (index >= input_len) {
        return input[index];
    }

    return input[index + 1];
}

void push_token(Token_t** tokens, size_t* t_len, size_t* t_capacity, Token_t token) {
    push((void**)tokens, t_len, t_capacity, sizeof(token), &token);
}

Token_t keywoard_to_token(char* identifier) {
    char* keys[] = {
        "if",
        "else",
        "func",
        "for",
        "return",
        "while",
    };

    TokenType_t values[] = {
        If,
        Else,
        Func,
        For,
        Return,
        While,
    };
    
    size_t len = sizeof(keys) / sizeof(char*);

    for (size_t i = 0; i < len; i++) {
        if (strcmp(identifier, keys[i]) == 0) {
            return new_token(values[i], NULL);
        }
    }

    Literal_t* lit = (Literal_t*)malloc(sizeof(Literal_t));
    if (lit == NULL) {
        printf("malloc failed!\n");
        exit(1);
    }

    if (strcmp(identifier, "true") == 0) {
        lit->b = true;
        return new_token(Bool, lit);
    }

    if (strcmp(identifier, "false") == 0) {
        lit->b = false;
        return new_token(Bool, lit);
    }

    lit->str = identifier;
    return new_token(Identifier, lit);
}

char* extract_identifier(char* input, size_t input_len, size_t* index) {
    char* ident = (char*)malloc(1 * sizeof(char));
    if (ident == NULL) {
        printf("malloc failed!\n");
        exit(1);
    }

    size_t i_capacity = 1;
    size_t i_len = 0;

    while (
        ('A' <= input[*index] && input[*index] <= 'Z') 
        || ('a' <= input[*index] && input[*index] <= 'z')
        || ('0' <= input[*index] && input[*index] <= '9')
        || '_' == input[*index]
    ) {
        push((void**)&ident, &i_len, &i_capacity, sizeof(char), &input[*index]);
        (*index)++;
        if (*index >= input_len) {
            break;
        }
    }

    char null_ch = '\0';
    push((void**)&ident, &i_len, &i_capacity, sizeof(char), &null_ch);

    return ident;
}

char* extract_num(char* input, size_t input_len, size_t* index, bool* is_float) {
    char* num = (char*)malloc(1 * sizeof(char));
    if (num == NULL) {
        printf("malloc failed!\n");
        exit(1);
    }

    size_t n_capacity = 1;
    size_t n_len = 0;

    while ('0' <= input[*index] && input[*index] <= '9') {
        push((void**)&num, &n_len, &n_capacity, sizeof(char), &input[*index]);
        (*index)++;
        if (*index >= input_len) {
            break;
        }
    }

    if (input[*index] == '.') {
        *is_float = true;
        (*index)++;

        char dot = '.';
        push((void**)&num, &n_len, &n_capacity, sizeof(char), &dot);

        while ('0' <= input[*index] && input[*index] <= '9') {
            push((void**)&num, &n_len, &n_capacity, sizeof(char), &input[*index]);
            (*index)++;
            if (*index >= input_len) {
                break;
            }
        }
    }

    char null_ch = '\0';
    push((void**)&num, &n_len, &n_capacity, sizeof(char), &null_ch);

    return num;
}

char* extract_string(char* input, size_t input_len, size_t* index) {
    (*index)++;

    char* str = (char*)malloc(1 * sizeof(char));
    if (str == NULL) {
        printf("malloc failed!\n");
        exit(1);
    }

    size_t s_capacity = 1;
    size_t s_len = 0;

    while (input[*index] != '"') {
        push((void**)&str, &s_len, &s_capacity, sizeof(char), &input[*index]);
        (*index)++;
        if (*index >= input_len) {
            printf("Expected `\"` after string!\n");
            exit(1);
        }
    }

    char null_ch = '\0';
    push((void**)&str, &s_len, &s_capacity, sizeof(char), &null_ch);

    return str;
}
