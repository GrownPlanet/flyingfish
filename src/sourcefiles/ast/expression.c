#include <stdio.h>

#include "ast/expression.h"

void free_expression(Expression_t* expr) {
    switch (expr->type) {
        case ExpressionType_Binary:
            free_expression(&expr->value.binary->left);
            free_expression(&expr->value.binary->right);
            free(expr->value.binary);
            break;
        case ExpressionType_Unary:
            free_expression(&expr->value.unary->operant);
            free(expr->value.unary);
            break;
        case ExpressionType_Literal:
            if (expr->value.literal->type == TokenType_Identifier) {
                free(expr->value.literal->value->id);
            }
            free(expr->value.literal);
            break;
    }
}

TokenType_t get_expression_in_type(Expression_t* expr) {
    switch (expr->type) {
        case ExpressionType_Binary: return expr->value.binary->in_type;
        case ExpressionType_Unary: return expr->value.unary->type;
        case ExpressionType_Literal: return expr->value.literal->type;
        default: printf("internal compiler error: unknown ExpressionType (= %d)\n", expr->type); return -1;
    }
}

TokenType_t get_expression_out_type(Expression_t* expr) {
    switch (expr->type) {
        case ExpressionType_Binary: return expr->value.binary->out_type;
        case ExpressionType_Unary: return expr->value.unary->type;
        case ExpressionType_Literal: return expr->value.literal->type;
        default: printf("internal compiler error: nknown ExpressionType (= %d)\n", expr->type); return -1;
    }
}
