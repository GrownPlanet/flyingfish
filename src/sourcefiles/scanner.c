#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array_utils.h"
#include "error.h"
#include "scanner.h"

char peek_char(char* input, size_t input_len, size_t index);
void push_token(Token_t** tokens, size_t* t_capacity, size_t* t_len, Token_t token);

Token_t keyword_to_token(char* identifier, size_t line);

char* extract_identifier(char* input, size_t input_len, size_t* index, size_t line);
char* extract_num(char* input, size_t input_len, size_t* index, bool* is_float, size_t line);
char* extract_string(char* input, size_t input_len, size_t* index, size_t line);

ScanResult_t scan(char* input, size_t input_len) {
    Token_t* tokens = (Token_t*)malloc(sizeof(Token_t));

    ScanResult_t fail_result = {
        .tokens = NULL, 
        .len = 0,
    };

    if (tokens == NULL) {
        report_error("malloc failed", 0);
        return fail_result;
    }

    size_t t_capacity = 1;
    size_t t_len = 0;
    size_t line = 0;

    for (size_t i = 0; i < input_len; i++) {
        char ch = input[i];
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
                push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_Bang, line, NULL));
                break;
            case '/':
                if (peek_char(input, input_len, i) == '/') {
                    while (input[i] != '\n') {
                        i++;
                    }
                } else {
                    push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_Slash, line, NULL));
                }
                break;
            case '=':
                if (peek_char(input, input_len, i) == '=') {
                    push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_EqualEqual, line, NULL));
                    i++;
                } else {
                    push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_Equal, line, NULL));
                }
                break;
            case '>':
                if (peek_char(input, input_len, i) == '=') {
                    push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_GreaterEqual, line, NULL));
                    i++;
                } else {
                    push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_Greater, line, NULL));
                }
                break;
            case '<':
                if (peek_char(input, input_len, i) == '=') {
                    push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_LesserEqual, line, NULL));
                    i++;
                } else {
                    push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_Lesser, line, NULL));
                }
                break;
            case '&':
                push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_Bang, line, NULL));
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
                char* str = extract_string(input, input_len, &i, line);
                Literal_t* lit = (Literal_t*)malloc(sizeof(Literal_t));
                if (lit == NULL) {
                    report_error("malloc failed", line);
                    return fail_result;
                }

                lit->str = str;
                push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_StringV, line, lit));
                break;
            }
            case '\'': {
                i++;
                Literal_t* lit = (Literal_t*)malloc(sizeof(Literal_t));
                if (lit == NULL) {
                    report_error("malloc failed", line);
                    return fail_result;
                }

                lit->ch = input[i];
                push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_CharV, line, lit));

                i++;
                if (input[i] != '\'') {
                    report_error("expected `'` after char", line);
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
                    char* num_str = extract_num(input, input_len, &i, &is_float, line);

                    Literal_t* lit = (Literal_t*)malloc(sizeof(Literal_t));
                    if (lit == NULL) {
                        report_error("malloc failed", line);
                        return fail_result;
                    }

                    if (is_float) {
                        lit->db = strtod(num_str, NULL);
                        push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_FloatV, line, lit));
                    } 
                    else {
                        lit->ln = atol(num_str);
                        push_token(&tokens, &t_len, &t_capacity, new_token(TokenType_IntV, line, lit));
                    }

                    free(num_str);

                    i--;

                    break;
                }

                // check if c is is alphabetic
                if (('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z')) {
                    char* identifier = extract_identifier(input, input_len, &i, line);
                    push_token(&tokens, &t_len, &t_capacity, keyword_to_token(identifier, line));

                    i--;

                    break;
                }

                report_error("unexpected character", line);
                break;
        }
    }

    ScanResult_t scan_result = {
        .tokens = tokens, 
        .len = t_len
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

Token_t keyword_to_token(char* identifier, size_t line) {
    char* keys[] = {
        "if",
        "else",
        "func",
        "for",
        "return",
        "while",
        "int", 
        "float", 
        "char", 
        "string", 
        "bool",
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
    };
    
    size_t len = sizeof(keys) / sizeof(char*);

    for (size_t i = 0;i < len; i++) {
        if (strcmp(identifier, keys[i]) == 0) {
            return new_token(values[i], line, NULL);
        }
    }

    Literal_t* lit = (Literal_t*)malloc(sizeof(Literal_t));
    if (lit == NULL) {
        report_error("malloc failed", line);
        return new_token(TokenType_Identifier, line, NULL);
    }

    if (strcmp(identifier, "true") == 0) {
        lit->b = true;
        return new_token(TokenType_BoolV, line, lit);
    }

    if (strcmp(identifier, "false") == 0) {
        lit->b = false;
        return new_token(TokenType_BoolV, line, lit);
    }

    lit->str = identifier;
    return new_token(TokenType_Identifier, line, lit);
}

char* extract_identifier(char* input, size_t input_len, size_t* index, size_t line) {
    char* ident = (char*)malloc(1 * sizeof(char));
    if (ident == NULL) {
        report_error("malloc failed", line);
        return "";
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

char* extract_num(char* input, size_t input_len, size_t* index, bool* is_float, size_t line) {
    char* num = (char*)malloc(1 * sizeof(char));
    if (num == NULL) {
        report_error("malloc failed", line);
        return "";
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

char* extract_string(char* input, size_t input_len, size_t* index, size_t line) {
    (*index)++;

    char* str = (char*)malloc(1 * sizeof(char));
    if (str == NULL) {
        report_error("malloc failed", line);
        return "";
    }

    size_t s_capacity = 1;
    size_t s_len = 0;

    while (input[*index] != '"') {
        push((void**)&str, &s_len, &s_capacity, sizeof(char), &input[*index]);
        (*index)++;
        if (*index >= input_len) {
            report_error("expected `\"` after string", line);
            return "";
        }
    }

    char null_ch = '\0';
    push((void**)&str, &s_len, &s_capacity, sizeof(char), &null_ch);

    return str;
}

void free_tokens(ScanResult_t tokens) {
    for (size_t i = 0; i < tokens.len; i++) {
        Token_t token = tokens.tokens[i];
        if (token.type == TokenType_StringV || token.type == TokenType_Identifier) {
            free(token.literal->str);
        }
        free(token.literal);
    }
    free(tokens.tokens);
}
