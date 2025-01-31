#include <stdint.h>
#include <stdio.h>

#include "interpreter.h"
#include "bytecode.h"
#include "token.h"

Instruction_t read_instruction(Interpreter_t* inter);
int exec_instr(Instruction_t instr, Interpreter_t* inter);
Literal_t read_number(Interpreter_t* inter);
AddressingMode_t read_addressing_mode(Interpreter_t* inter);
TokenType_t read_type(Interpreter_t* inter);
int set_stack(Stack_t* stack, size_t index, Literal_t element);
Literal_t get_elem(Interpreter_t* inter, int64_t pos);
int exec_binary(
    Interpreter_t* inter, Literal_t (*operation)(Literal_t, Literal_t) // higher order function
);

Interpreter_t new_interpreter(unsigned char* code, size_t len) {
    Stack_t stack = {
        .data = (Literal_t*)malloc(sizeof(Literal_t)),
        .capacity = 1,
    };

    return (Interpreter_t){
        .code = code,
        .len = len,
        .instr_ptr = 0,
        .stack = stack,
    };
}

int interpret(Interpreter_t interpreter) {
    while (interpreter.instr_ptr < interpreter.len) {
        Instruction_t instr = read_instruction(&interpreter);
        int res = exec_instr(instr, &interpreter);
        if (res == 1) { return res; }
    }

    printf("stack:\n");
    for (size_t i = 0; i < interpreter.stack.capacity; i++) {
        Literal_t sd = interpreter.stack.data[i];
        printf("  %lx; %f; %ld,\n", sd.i, sd.f, sd.i);
    }

    return 0;
}

// arguments for the exec_binary function
Literal_t add_i(Literal_t n1, Literal_t n2) { return (Literal_t) { n1.i + n2.i }; }
Literal_t sub_i(Literal_t n1, Literal_t n2) { return (Literal_t) { n1.i - n2.i }; }
Literal_t mul_i(Literal_t n1, Literal_t n2) { return (Literal_t) { n1.i * n2.i }; }
Literal_t div_i(Literal_t n1, Literal_t n2) { return (Literal_t) { n1.i / n2.i }; }

Literal_t add_f(Literal_t n1, Literal_t n2) { 
    Literal_t s;
    s.f = n1.f + n2.f;
    return s;
}
Literal_t sub_f(Literal_t n1, Literal_t n2) { return (Literal_t) { n1.f - n2.f }; }
Literal_t mul_f(Literal_t n1, Literal_t n2) { return (Literal_t) { n1.f * n2.f }; }
Literal_t div_f(Literal_t n1, Literal_t n2) { return (Literal_t) { n1.f / n2.f }; }

int exec_instr(Instruction_t instr, Interpreter_t* inter) {
    switch (instr) {
        case Instruction_Add: {
            TokenType_t t = read_type(inter);
            int res = 0;
            switch (t) {
                case TokenType_IntV: { res = exec_binary(inter, add_i); break; }
                case TokenType_FloatV: { res = exec_binary(inter, add_f); break; }
                default: { printf("Wrong type for instruction: %d\n", t); res = 1; };
            }
            if (res == 1) { return res; }
            break;
        }
        case Instruction_Div: {
            TokenType_t t = read_type(inter);
            int res = 0;
            switch (t) {
                case TokenType_IntV: { res = exec_binary(inter, div_i); break; }
                case TokenType_FloatV: { res = exec_binary(inter, div_f); break; }
                default: { printf("Wrong type for instruction: %d\n", t); res = 1; };
            }
            if (res == 1) { return res; }
            break;
        }
        case Instruction_Mov: {
            Literal_t op1 = read_number(inter);
            AddressingMode_t adr2 = read_addressing_mode(inter);
            Literal_t op2 = read_number(inter);

            Literal_t num;
            switch (adr2) {
                case AddressingMode_Direct:   num = op2; break;
                case AddressingMode_Indirect: num = get_elem(inter, op2.i); break;
                default: printf("(unreachable) unkown addressing mode!\n"); return 1; break;
            }
            int res = set_stack(&inter->stack, (size_t)op1.i, num);
            if (res == 1) { return res; }
            break;
        }
        case Instruction_Mul: {
            TokenType_t t = read_type(inter);
            int res = 0;
            switch (t) {
                case TokenType_IntV: { res = exec_binary(inter, mul_i); break; }
                case TokenType_FloatV: { res = exec_binary(inter, mul_f); break; }
                default: { printf("Wrong type for instruction: %d\n", t); res = 1; };
            }
            if (res == 1) { return res; }
            break;
        }
        case Instruction_Neg: {
            Literal_t op1 = read_number(inter);
            TokenType_t t = read_type(inter);
            Literal_t n;
            switch (t) {
                case TokenType_IntV: {
                    n.i = -get_elem(inter, op1.i).i;
                    break;
                }
                case TokenType_FloatV: {
                    n.f = -get_elem(inter, op1.f).f;
                    break;
                }
                default:
                    printf("TODO: proper error handeling.. :(\n");
                    return 1;
                    break;
            }
            int res = set_stack(&inter->stack, (size_t)op1.i, n);
            if (res == 1) { return res; }
            break;
        }
        case Instruction_Sub: {
            TokenType_t t = read_type(inter);
            int res = 0;
            switch (t) {
                case TokenType_IntV: { res = exec_binary(inter, sub_i); break; }
                case TokenType_FloatV: { res = exec_binary(inter, sub_f); break; }
                default: { printf("Wrong type for instruction: %d\n", t); res = 1; };
            }
            if (res == 1) { return res; }
            break;
        }
        default:
            printf("TODO: instruction %d not programmed yet!\n", instr);
            return 1;
            break;
    }
    return 0;
}

int exec_binary(
    Interpreter_t* inter, Literal_t (*operation)(Literal_t, Literal_t) // higher order function
) {
    Literal_t op1 = read_number(inter);
    AddressingMode_t adr2 = read_addressing_mode(inter);
    Literal_t op2 = read_number(inter);

    Literal_t num;
    switch (adr2) {
        case AddressingMode_Direct: num = operation(get_elem(inter, op1.i), op2); break;
        case AddressingMode_Indirect: num = operation(get_elem(inter, op1.i), get_elem(inter, op2.i)); break;
        default: printf("(unreachable) unkown addressing mode!\n"); return 1; break;
    }
    int res = set_stack(&inter->stack, (size_t)op1.i, num);
    return res;
}

Literal_t read_number(Interpreter_t* inter) {
    int64_t num = 0;
    for (size_t i = 0; i < sizeof(Literal_t); i++) {
        num = num | ((int64_t)inter->code[inter->instr_ptr + i] << (i * 8));
    }
    inter->instr_ptr += sizeof(Literal_t);
    return (Literal_t){num};
}

AddressingMode_t read_addressing_mode(Interpreter_t* inter) {
    int adrm = 0;
    for (size_t i = 0; i < sizeof(AddressingMode_t); i++) {
        adrm = adrm | (inter->code[inter->instr_ptr  + i] << (i * 8));
    }
    inter->instr_ptr += sizeof(AddressingMode_t);
    return (AddressingMode_t)adrm;
}

TokenType_t read_type(Interpreter_t* inter) {
    int t = 0;
    for (size_t i = 0; i < sizeof(TokenType_t); i++) {
        t = t | (inter->code[inter->instr_ptr  + i] << (i * 8));
    }
    inter->instr_ptr += sizeof(TokenType_t);
    return (TokenType_t)t;
}

Instruction_t read_instruction(Interpreter_t* inter) {
    int instruction = 0;
    for (size_t i = 0; i < sizeof(Instruction_t); i++) {
        instruction = instruction | (inter->code[inter->instr_ptr  + i] << (i * 8));
    }
    inter->instr_ptr += sizeof(Instruction_t);
    return (Instruction_t)instruction;
}

Literal_t get_elem(Interpreter_t* inter, int64_t pos) {
    return inter->stack.data[(size_t)pos];
}

int set_stack(Stack_t* stack, size_t index, Literal_t element) {
    while (index >= stack->capacity) {
        stack->capacity *= 2;
        stack->data = realloc(stack->data, stack->capacity * sizeof(Literal_t));

        if (stack->data == NULL) {
            printf("Realloc failed!\n");
            return 1;
        }
    }

    stack->data[index] = element;
    return 0;
}
