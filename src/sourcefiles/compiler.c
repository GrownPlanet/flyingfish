#include <stdio.h>

#include "bytecode.h"
#include "compiler.h"
#include "expression.h"

Instructions_t* compile_literal(EV_Literal_t* literal) {

}

Instructions_t* compile_expression(Expression_t* expr) {
    switch (expr->type) {
        case ExpressionType_Literal:
            return compile_literal(expr->value.literal);
            break;
        default:
            printf("TODO1 compiler.c\n");
            exit(0);
            break;
    }
}

Instructions_t* compile(Expression_t* expr) {
    char* data = (char*)malloc(sizeof(char));
    Instructions_t instructions = {
        data, 0, 1,
    };

    return compile_expression(expr);
}
