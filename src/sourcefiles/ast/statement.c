#include <stdio.h>

#include "statement.h"
#include "expression.h"
#include "token.h"

void free_statement(Statement_t* expr) {
    switch (expr->type) {
        case StatementType_Print: free_expression(expr->value.print->expr); break;
        case StatementType_Var: free_expression(expr->value.var->expr); break;
        case StatementType_Assignment: free_expression(expr->value.assignment->expr); break;
        default: printf("Unknown expression type: %d\n", expr->type); break;
    }
}
