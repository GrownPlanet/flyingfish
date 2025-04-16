#include <stdio.h>

#include "numtypes.h"
#include "parser.h"
#include "expression.h"
#include "statement.h"
#include "token.h"
#include "hashmap.h"

typedef struct {
    Token_t* tokens;
    size_t len;
    size_t index;
    HashMap_t hashmap; // this also contains a value type that wont be used, but it's better than
                       // writing a new hashmap
} Parser_t;

Expression_t* parse_expr(Parser_t* parser);
Statement_t* parse_statement(Parser_t* parser);
void advance(Parser_t* parser);

Expression_t* parse_primary(Parser_t* parser) {
    // literal
    Token_t token = parser->tokens[parser->index];

    switch (token.type) {
        case TokenType_IntV:
        case TokenType_FloatV:
        case TokenType_CharV:
        case TokenType_StringV:
        case TokenType_BoolV: {
            Expression_t* expr = (Expression_t*)malloc(sizeof(Expression_t));
            if (expr == NULL) {
                printf("malloc failed!\n");
                return NULL;
            }
            expr->line = token.line;
            expr->type = ExpressionType_Literal;

            EV_Literal_t* lit = (EV_Literal_t*)malloc(sizeof(EV_Literal_t));
            if (lit == NULL) {
                printf("malloc failed!\n");
                return NULL;
            }

            lit->type = token.type;
            lit->value = token.literal;

            ExpressionValue_t v;
            v.literal = lit;
            expr->value = v;
            advance(parser);

            return expr;
        }
        case TokenType_Identifier: {
            Expression_t* expr = (Expression_t*)malloc(sizeof(Expression_t));
            if (expr == NULL) {
                printf("malloc failed!\n");
                return NULL;
            }
            expr->line = token.line;
            expr->type = ExpressionType_Literal;

            EV_Literal_t* lit = (EV_Literal_t*)malloc(sizeof(EV_Literal_t));
            if (lit == NULL) {
                printf("malloc failed!\n");
                return NULL;
            }

            lit->type = token.type;
            HM_GetResult_t res = hashmap_get(&parser->hashmap, *token.literal->s);
            if (res.had_error) {
                printf("error: unkown variable: ");
                string_print(*token.literal->s);
                printf("\n");
                return NULL;
            }
            Identifier_t* id = (Identifier_t*)malloc(sizeof(Identifier_t));
            id->name = *token.literal->s;
            id->type = res.type;

            Literal_t* literal = (Literal_t*)malloc(sizeof(Literal_t));
            literal->id = id;
            lit->value = literal;

            ExpressionValue_t v;
            v.literal = lit;
            expr->value = v;
            advance(parser);

            return expr;
        }
        case TokenType_LeftParen: {
            advance(parser);
            Expression_t* expr = parse_expr(parser);

            token = parser->tokens[parser->index];
            if (token.type != TokenType_RightParen) {
                printf("error: expected right paren after expression on line %" PRIu "\n", token.line);
                return NULL;
            }
            advance(parser);

            return expr;
        }
        default:
            printf("error: expected expression on line %" PRIu ", got `", token.line);
            print_token_type(token.type);
            printf("` instead\n");
            return NULL;
    }
}

Expression_t* parse_unary(Parser_t* parser) {
    // -[unary | primary]
    Token_t token = parser->tokens[parser->index];

    if (token.type == TokenType_Bang || token.type == TokenType_Minus) {
        advance(parser);

        Expression_t* expr = (Expression_t*)malloc(sizeof(Expression_t));
        if (expr == NULL) {
            printf("malloc failed!\n");
            return NULL;
        }
        expr->line = token.line;

        expr->type = ExpressionType_Unary;

        EV_Unary_t* un = (EV_Unary_t*)malloc(sizeof(EV_Unary_t));
        if (un == NULL) {
            printf("malloc failed!\n");
            return NULL;
        }
        un->operator = token.type;

        Expression_t* operant = parse_unary(parser);
        if (operant == NULL) {
            return NULL;
        }
        un->operant = *operant;

        un->type = get_expression_out_type(operant);

        ExpressionValue_t v;
        v.unary = un;
        expr->value = v;

        return expr;
    }

    return parse_primary(parser);
}

Expression_t* parse_binary(
    Parser_t* parser, TokenType_t* types, size_t types_len,
    bool changetype, TokenType_t changetypeto,
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
            printf("malloc failed!\n");
            return NULL;
        }
        right->line = token.line;

        right->type = ExpressionType_Binary;

        EV_Binary_t* bin = (EV_Binary_t*)malloc(sizeof(EV_Binary_t));
        if (bin == NULL) {
            printf("malloc failed!\n");
            return NULL;
        }

        bin->left = *expr;
        bin->operator = token;

        advance(parser);
        token = parser->tokens[parser->index];

        bin->right = *base_func(parser);
        token = parser->tokens[parser->index];

        TokenType_t t1 = get_expression_out_type(expr);
        if (t1 == TokenType_Identifier) {
            t1 = expr->value.literal->value->id->type;
        }
        TokenType_t t2 = get_expression_out_type(&bin->right);
        if (t2 == TokenType_Identifier) {
            t2 = bin->right.value.literal->value->id->type;
        }
        if (t1 != t2) {
            printf("error: types do not match in expression: ");
            print_token_type(t1);
            printf(" and ");
            print_token_type(t2);
            printf(" on line %" PRIu "\n", token.line);
            return NULL;
        }
        bin->in_type = t1;
        bin->out_type = changetype ? changetypeto : t1;

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
        printf("malloc failed!\n");
        return NULL;
    }
    types[0] = TokenType_Star;
    types[1] = TokenType_Slash;

    return parse_binary(parser, types, types_len, false, 0, parse_unary);
}

Expression_t* parse_term(Parser_t* parser) {
    // + or -
    size_t types_len = 2;
    TokenType_t* types = (TokenType_t*)malloc(sizeof(TokenType_t) * types_len);
    if (types == NULL) {
        printf("malloc failed!\n");
        return NULL;
    }
    types[0] = TokenType_Plus;
    types[1] = TokenType_Minus;
    return parse_binary(parser, types, types_len, false, 0, parse_factor);
}

Expression_t* parse_comparison(Parser_t* parser) {
    // > or >= or < or <=
    size_t types_len = 4;
    TokenType_t* types = (TokenType_t*)malloc(sizeof(TokenType_t) * types_len);
    if (types == NULL) {
        printf("malloc failed!\n");
        return NULL;
    }
    types[0] = TokenType_Greater;
    types[1] = TokenType_GreaterEqual;
    types[2] = TokenType_Lesser;
    types[3] = TokenType_LesserEqual;
    return parse_binary(parser, types, types_len, true, TokenType_BoolV, parse_term);
}

Expression_t* parse_eq_neq(Parser_t* parser) {
    // == or !=
    size_t types_len = 2;
    TokenType_t* types = (TokenType_t*)malloc(sizeof(TokenType_t) * types_len);
    if (types == NULL) {
        printf("malloc failed!\n");
        return NULL;
    }
    types[0] = TokenType_EqualEqual;
    types[1] = TokenType_BangEqual;
    return parse_binary(parser, types, types_len, true, TokenType_BoolV, parse_comparison);
}

Expression_t* parse_expr(Parser_t* parser) {
    // && or ||
    size_t types_len = 2;
    TokenType_t* types = (TokenType_t*)malloc(sizeof(TokenType_t) * types_len);
    if (types == NULL) {
        printf("malloc failed!\n");
        return NULL;
    }
    types[0] = TokenType_And;
    types[1] = TokenType_Or;
    return parse_binary(parser, types, types_len, true, TokenType_BoolV, parse_eq_neq);
}

Statement_t* parse_print(Parser_t* parser) {
    advance(parser);

    Token_t token = parser->tokens[parser->index];
    if (token.type != TokenType_LeftParen) {
        printf("error: expected left paren after `print` on line %" PRIu "\n", token.line);
        return NULL;
    }
    advance(parser);

    Statement_t* stmt = (Statement_t*)malloc(sizeof(Statement_t));
    if (stmt == NULL) { printf("malloc failed!\n"); return NULL; }
    stmt->type = StatementType_Print;

    ST_Print_t* print = (ST_Print_t*)malloc(sizeof(ST_Print_t));
    if (stmt == NULL) { printf("malloc failed!\n"); return NULL; }
    print->expr = parse_expr(parser);
    if (print->expr == NULL) { return NULL; }

    StatementValue_t v;
    v.print = print;
    stmt->value = v;

    token = parser->tokens[parser->index];
    if (token.type != TokenType_RightParen) {
        printf("error: expected right paren after print statement on line %" PRIu "\n", token.line);
        return NULL;
    }
    advance(parser);

    token = parser->tokens[parser->index];
    if (token.type != TokenType_Semicolon) {
        printf("error: expected semicolon after print statement on line %" PRIu "\n", token.line);
        return NULL;
    }
    advance(parser);
    
    return stmt;
}

Statement_t* parse_var(Parser_t* parser) {
    advance(parser);

    Statement_t* stmt = (Statement_t*)malloc(sizeof(Statement_t));
    if (stmt == NULL) { printf("malloc failed!\n"); return NULL; }

    stmt->type = StatementType_Var;

    ST_Var_t* var = (ST_Var_t*)malloc(sizeof(ST_Var_t));
    if (var == NULL) { printf("malloc failed!\n"); return NULL; }

    Token_t token = parser->tokens[parser->index];
    if (token.type != TokenType_Identifier) {
        printf("error: expected identifier after `var` on line %" PRIu "\n", token.line);
        return NULL;
    }
    var->name = token.literal->s;
    advance(parser);

    token = parser->tokens[parser->index];
    if (token.type != TokenType_Equal) {
        printf("error: expected `=` in variable assignment\n");
        return NULL;
    }
    advance(parser);

    var->expr = parse_expr(parser);

    StatementValue_t v;
    v.var = var;
    stmt->value = v;

    token = parser->tokens[parser->index];
    if (token.type != TokenType_Semicolon) {
        printf("error: expected semicolon after print statement on line %" PRIu "\n", token.line);
        return NULL;
    }
    advance(parser);

    hashmap_insert(&parser->hashmap, *var->name, -1, get_expression_out_type(var->expr));

    return stmt;
}

Statement_t* parse_assignment(Parser_t* parser) {
    Statement_t* stmt = (Statement_t*)malloc(sizeof(Statement_t));
    if (stmt == NULL) { printf("malloc failed!\n"); return NULL; }

    stmt->type = StatementType_Assignment;

    ST_Assignment_t* assig = (ST_Assignment_t*)malloc(sizeof(ST_Assignment_t));
    if (assig == NULL) { printf("malloc failed!\n"); return NULL; }

    Token_t token = parser->tokens[parser->index];
    assig->name = token.literal->s;
    advance(parser);

    token = parser->tokens[parser->index];
    if (token.type != TokenType_Equal) {
        printf("error: expected `=` in variable assignment\n");
        return NULL;
    }
    advance(parser);

    assig->expr = parse_expr(parser);

    StatementValue_t v;
    v.assignment = assig;
    stmt->value = v;

    token = parser->tokens[parser->index];
    if (token.type != TokenType_Semicolon) {
        printf("error: expected semicolon after print statement on line %" PRIu "\n", token.line);
        return NULL;
    }
    advance(parser);

    return stmt;
}

Statement_t* parse_block(Parser_t* parser) {
    advance(parser);

    Statement_t* statements = (Statement_t*)malloc(sizeof(Statement_t));
    if (statements == NULL) {
        printf("malloc failed!\n");
        return NULL;
    }

    size_t capacity = 1;
    size_t len = 0;

    while (parser->tokens[parser->index].type != TokenType_RightBrace) {
        Statement_t* statement = parse_statement(parser);
        if (statement == NULL) {
            return NULL;
        }
        
        if (len >= capacity) {
            capacity *= 2;
            statements = realloc(statements, sizeof(Statement_t) * capacity);
            if (statements == NULL) {
                printf("realloc failed!\n");
                return NULL;
            }
        }

        statements[len] = *statement;
        free(statement);
        len++;
        if (parser->index >= parser->len) {
            printf("error: expected `}` after a blcok\n");
            return NULL;
        }
    }
    advance(parser);
    statements = (Statement_t*)realloc(statements, sizeof(Statement_t) * len);
    if (statements == NULL) {
        printf("realloc failed!\n");
        return NULL;
    }

    ST_Block_t* block = (ST_Block_t*)malloc(sizeof(ST_Block_t));
    if (block == NULL) {
        printf("malloc failed!\n");
        return NULL;
    }
    block->stmts = statements;
    block->len = len;

    Statement_t* stmt = (Statement_t*)malloc(sizeof(Statement_t));
    if (stmt == NULL) {
        printf("malloc failed!\n");
        return NULL;
    }
    stmt->type = StatementType_Block;
    StatementValue_t v;
    v.block = block;
    stmt->value = v;

    return stmt;
}

Statement_t* parse_if(Parser_t* parser) {
    advance(parser);

    Statement_t* stmt = (Statement_t*)malloc(sizeof(Statement_t));
    if (stmt == NULL) { printf("malloc failed!\n"); return NULL; }

    stmt->type = StatementType_If;

    ST_If_t* ifs = (ST_If_t*)malloc(sizeof(Statement_t));
    if (ifs == NULL) { printf("malloc failed!\n"); return NULL; }

    ifs->expr = parse_expr(parser);
    if (ifs->expr == NULL) { return NULL; }

    ifs->then = parse_statement(parser);
    if (ifs->then == NULL) { return NULL; }

    StatementValue_t v;
    v.ifs = ifs;
    stmt->value = v;

    return stmt;
}

Statement_t* parse_statement(Parser_t* parser) {
    Token_t token = parser->tokens[parser->index];
    Statement_t* stmt;

    switch (token.type) {
        case TokenType_Print: stmt = parse_print(parser); break;
        case TokenType_Var: stmt = parse_var(parser); break;
        case TokenType_Identifier: stmt = parse_assignment(parser); break;
        case TokenType_LeftBrace: stmt = parse_block(parser); break;
        case TokenType_If: stmt = parse_if(parser); break;
        default: 
            printf("error: unexpected token in a statement on line %" PRIu ": ", token.line); 
            print_token_type(token.type);
            printf("\n"); 
            break;
    }

    return stmt;
}

ParseResult_t parse(ScanResult_t tokens) {
    Parser_t parser = {
        .tokens = tokens.tokens,
        .len = tokens.len,
        .index = 0,
        .hashmap = new_hashmap(),
    };

    Statement_t* statements = (Statement_t*)malloc(sizeof(Statement_t));
    if (statements == NULL) {
        printf("malloc failed!\n");
        free(parser.hashmap.data);
        return (ParseResult_t) { .statements = NULL, .len = 0, .had_error = true };
    }

    size_t capacity = 1;
    size_t len = 0;

    while (parser.index < parser.len) {
        Statement_t* statement = parse_statement(&parser);
        if (statement == NULL) {
            free(parser.hashmap.data);
            return (ParseResult_t) { .statements = NULL, .len = 0, .had_error = true };
        }
        
        if (len >= capacity) {
            capacity *= 2;
            statements = realloc(statements, sizeof(Statement_t) * capacity);
            if (statements == NULL) {
                printf("realloc failed!\n");
                free(parser.hashmap.data);
                return (ParseResult_t) { .statements = NULL, .len = 0, .had_error = true };
            }
        }

        statements[len] = *statement;
        free(statement);
        len++;
    }
    statements = (Statement_t*)realloc(statements, sizeof(Statement_t) * len);
    if (statements == NULL) {
        printf("realloc failed!\n");
        free(parser.hashmap.data);
        return (ParseResult_t) { .statements = NULL, .len = 0, .had_error = true };
    }

    free(parser.hashmap.data);
    return (ParseResult_t) { .statements = statements, .len = len, .had_error = false };
}

void advance(Parser_t* parser) {
    if (parser->index < parser->len) {
        (parser->index)++;
    }
}
