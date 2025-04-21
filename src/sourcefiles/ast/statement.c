#include <stdio.h>

#include "ast/statement.h"
#include "ast/expression.h"
#include "token.h"

void free_statement(Statement_t* stmt) {
    switch (stmt->type) {
        case StatementType_Print: free_expression(stmt->value.print->expr); break;
        case StatementType_Var: free_expression(stmt->value.var->expr); break;
        case StatementType_Assignment: free_expression(stmt->value.assignment->expr); break;
        case StatementType_Block: {
            ST_Block_t* block = stmt->value.block;
            for (size_t i = 0; i < block->len; i++) {
                free_statement(&block->stmts[i]);
            }
            free(block->stmts);
            break;
        }
        case StatementType_If:
            free_expression(stmt->value.if_s->condition);
            free_statement(stmt->value.if_s->if_body);
            if (stmt->value.if_s->else_body != NULL) {
                free_statement(stmt->value.if_s->else_body);
            }
            break;
        case StatementType_While:
            free_expression(stmt->value.while_s->condition);
            free_statement(stmt->value.while_s->body);
            break;
        case StatementType_For:
            free_statement(stmt->value.for_s->init);
            free_expression(stmt->value.for_s->condition);
            free_statement(stmt->value.for_s->incr);
            free_statement(stmt->value.for_s->body);
            break;
        default: printf("internal compiler error: unknown statement type: %d\n", stmt->type); break;
    }
}
