#include "expression.h"

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
            free(expr->value.literal);
            break;
    }
}
