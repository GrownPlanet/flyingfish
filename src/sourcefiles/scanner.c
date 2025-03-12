#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "numtypes.h"
#include "array_utils.h"
#include "scanner.h"

char peek_char(String_t input, size_t index);
void push_token(Token_t** tokens, size_t* t_capacity, size_t* t_len, Token_t token);

Token_t keyword_to_token(String_t* identifier, size_t line);

String_t* extract_identifier(String_t input, size_t* index);
char* extract_num(String_t input, size_t* index, bool* is_float);
String_t* extract_string(String_t input, size_t* index, size_t line);

ScanResult_t scan(String_t input) {
    Token_t* tokens = (Token_t*)malloc(sizeof(Token_t));
    bool had_error = false;

    if (tokens == NULL) {
        printf("Malloc failed\n");
        return (ScanResult_t){
            .tokens = NULL,
            .len = 0,
            .had_error = true,
        };
    }

    size_t t_capacity = 1;
    size_t t_len = 0;
    size_t line = 1;

    for (size_t i = 0; i < input.len; i++) {
        char ch = input.chars[i];
        switch (ch) {
            case '(':
                push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_LeftParen, line, NULL));
                break;
            case ')':
                push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_RightParen, line, NULL));
                break;
            case '{':
                push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_LeftBrace, line, NULL));
                break;
            case '}':
                push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_RightBrace, line, NULL));
                break;
            case '+':
                push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_Plus, line, NULL));
                break;
            case '-':
                push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_Minus, line, NULL));
                break;
            case '*':
                push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_Star, line, NULL));
                break;
            case '!':
                if (peek_char(input, i) == '=') {
                    push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_BangEqual, line, NULL));
                    i++;
                } else {
                    push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_Bang, line, NULL));
                }
                break;
                break;
            case '/':
                if (peek_char(input, i) == '/') {
                    while (input.chars[i] != '\n') {
                        i++;
                    }
                } else {
                    push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_Slash, line, NULL));
                }
                break;
            case '=':
                if (peek_char(input, i) == '=') {
                    push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_EqualEqual, line, NULL));
                    i++;
                } else {
                    push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_Equal, line, NULL));
                }
                break;
            case '>':
                if (peek_char(input, i) == '=') {
                    push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_GreaterEqual, line, NULL));
                    i++;
                } else {
                    push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_Greater, line, NULL));
                }
                break;
            case '<':
                if (peek_char(input, i) == '=') {
                    push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_LesserEqual, line, NULL));
                    i++;
                } else {
                    push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_Lesser, line, NULL));
                }
                break;
            case '&':
                push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_And, line, NULL));
                break;
            case '|':
                push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_Or, line, NULL));
                break;
            case ',':
                push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_Comma, line, NULL));
                break;
            case '.':
                push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_Point, line, NULL));
                break;
            case ';':
                push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_Semicolon, line, NULL));
                break;
            case '"': {
                String_t* str = extract_string(input, &i, line);
                Literal_t* lit = (Literal_t*)malloc(sizeof(Literal_t));
                if (lit == NULL) {
                    printf("Malloc failed\n");
                    return (ScanResult_t){
                        .tokens = NULL,
                        .len = 0,
                        .had_error = true,
                    };
                }

                lit->s = str;
                push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_StringV, line, lit));
                break;
            }
            case '\'': {
                i++;
                Literal_t* lit = (Literal_t*)malloc(sizeof(Literal_t));
                if (lit == NULL) {
                    printf("Malloc failed\n");
                    return (ScanResult_t){
                        .tokens = NULL,
                        .len = 0,
                        .had_error = true,
                    };
                }

                lit->c = input.chars[i];
                push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_CharV, line, lit));

                i++;
                if (input.chars[i] != '\'') {
                    printf("Expected `'` after char on line %" PRIu "\n", line);
                    had_error = true;
                    i--;
                }

                break;
            }
            case '\n':
                line++;
                break;
            case '\r':
            case ' ':
                break;
            default:
                // check if c is numeric
                if ('0' <= ch && ch <= '9') {
                    bool is_float = false;
                    char* num_str = extract_num(input, &i, &is_float);

                    Literal_t* lit = (Literal_t*)malloc(sizeof(Literal_t));
                    if (lit == NULL) {
                        printf("Malloc failed\n");
                        return (ScanResult_t){
                            .tokens = NULL,
                            .len = 0,
                            .had_error = true,
                        };
                    }

                    if (is_float) {
                        lit->f = strtod(num_str, NULL);
                        push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_FloatV, line, lit));
                    } else {
                        lit->i = atol(num_str);
                        push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_IntV, line, lit));
                    }

                    free(num_str);

                    i--;

                    break;
                }

                // check if c is is alphabetic
                if (('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z')) {
                    String_t* identifier = extract_identifier(input, &i);
                    push_token(&tokens, &t_len, &t_capacity, keyword_to_token(identifier, line));

                    i--;

                    break;
                }

                printf("Unexpected character (= %c) on line %" PRIu "\n", ch, line);
                had_error = true;
                break;
        }
    }

    return (ScanResult_t) {
        .tokens = tokens, 
        .len = t_len,
        .had_error = had_error,
    };
}

char peek_char(String_t input, size_t index) {
    if (index >= input.len) {
        return input.chars[index];
    }

    return input.chars[index + 1];
}

void push_token(Token_t** tokens, size_t* t_len, size_t* t_capacity, Token_t token) {
    push((void**)tokens, t_len, t_capacity, sizeof(token), &token);
}

Token_t keyword_to_token(String_t* identifier, size_t line) {
    String_t keys[] = {
        string_from_chptr("if"),
        string_from_chptr("else"),
        string_from_chptr("func"),
        string_from_chptr("for"),
        string_from_chptr("return"),
        string_from_chptr("while"),
        string_from_chptr("int"),
        string_from_chptr("float"),
        string_from_chptr("char"),
        string_from_chptr("string"),
        string_from_chptr("bool"),
        string_from_chptr("print"),
        string_from_chptr("var"),
    };

    TokenType_t values[] = {
        TokenType_If,
        TokenType_Else,
        TokenType_Func,
        TokenType_For,
        TokenType_Return,
        TokenType_While,
        TokenType_IntT,
        TokenType_FloatT,
        TokenType_CharT,
        TokenType_StringT,
        TokenType_BoolT,
        TokenType_Print,
        TokenType_Var,
    };

    size_t len = sizeof(keys) / sizeof(String_t);

    for (size_t i = 0; i < len; i++) {
        if (string_cmp(*identifier, keys[i])) {
            return new_token(values[i], line, NULL);
        }
    }

    Literal_t* lit = (Literal_t*)malloc(sizeof(Literal_t));
    if (lit == NULL) {
        printf("Malloc failed\n");
        return new_token(TokenType_Identifier, line, NULL);
    }

    if (string_cmp(*identifier, (String_t) { .chars = "true", .len = 4 })) {
        lit->b = true;
        return new_token(TokenType_BoolV, line, lit);
    }

    if (string_cmp(*identifier, (String_t) { .chars = "false", .len = 5 })) {
        lit->b = false;
        return new_token(TokenType_BoolV, line, lit);
    }

    lit->s = identifier;
    return new_token(TokenType_Identifier, line, lit);
}

String_t* extract_identifier(String_t input, size_t* index) {
    char* ident = (char*)malloc(1 * sizeof(char));
    if (ident == NULL) {
        printf("Malloc failed\n");
        return NULL;
    }

    size_t i_capacity = 1;
    size_t i_len = 0;

    while (
        ('A' <= input.chars[*index] && input.chars[*index] <= 'Z') 
        || ('a' <= input.chars[*index] && input.chars[*index] <= 'z')
        || ('0' <= input.chars[*index] && input.chars[*index] <= '9')
        || '_' == input.chars[*index]
    ) {
        push((void**)&ident, &i_len, &i_capacity, sizeof(char), &input.chars[*index]);
        (*index)++;
        if (*index >= input.len) {
            break;
        }
    }

    ident = realloc((void*)ident, i_len);

    String_t* retv = (String_t*)malloc(sizeof(String_t));
    retv->chars = ident;
    retv->len = i_len;

    return retv;
}

char* extract_num(String_t input, size_t* index, bool* is_float) {
    char* num = (char*)malloc(1 * sizeof(char));
    if (num == NULL) {
        printf("Malloc failed\n");
        return NULL;
    }

    size_t n_capacity = 1;
    size_t n_len = 0;

    while ('0' <= input.chars[*index] && input.chars[*index] <= '9') {
        push((void**)&num, &n_len, &n_capacity, sizeof(char), &input.chars[*index]);
        (*index)++;
        if (*index >= input.len) {
            break;
        }
    }

    if (input.chars[*index] == '.') {
        *is_float = true;
        (*index)++;

        char dot = '.';
        push((void**)&num, &n_len, &n_capacity, sizeof(char), &dot);

        while ('0' <= input.chars[*index] && input.chars[*index] <= '9') {
            push((void**)&num, &n_len, &n_capacity, sizeof(char), &input.chars[*index]);
            (*index)++;
            if (*index >= input.len) {
                break;
            }
        }
    }

    char null_ch = '\0';
    push((void**)&num, &n_len, &n_capacity, sizeof(char), &null_ch);

    return num;
}

String_t* extract_string(String_t input, size_t* index, size_t line) {
    (*index)++;

    char* str = (char*)malloc(1 * sizeof(char));
    if (str == NULL) {
        printf("Malloc failed\n");
        return NULL;
    }

    size_t s_capacity = 1;
    size_t s_len = 0;

    while (input.chars[*index] != '"') {
        push((void**)&str, &s_len, &s_capacity, sizeof(char), &input.chars[*index]);
        (*index)++;
        if (*index >= input.len) {
            printf("Expected `\"` after string on line %" PRIu "\n", line);
            return NULL;
        }
    }

    str = realloc((void*)str, s_len);

    String_t* retv = (String_t*)malloc(sizeof(String_t));
    retv->chars = str;
    retv->len = s_len;

    return retv;
}

void free_tokens(ScanResult_t tokens) {
    for (size_t i = 0; i < tokens.len; i++) {
        Token_t token = tokens.tokens[i];
        if (token.type == TokenType_StringV || token.type == TokenType_Identifier) {
            free(token.literal->s->chars);
            free(token.literal->s);
        }
        free(token.literal);
    }
    free(tokens.tokens);
}
