#include <stdio.h>

#include "statement.h"
#include "expression.h"
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
            free_expression(stmt->value.ifs->expr);
            free_statement(stmt->value.ifs->then);
            break;
        default: printf("internal compiler error: unknown statement type: %d\n", stmt->type); break;
    }
}
