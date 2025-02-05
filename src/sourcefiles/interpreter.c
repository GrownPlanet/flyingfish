#include <stdint.h>
#include <stdio.h>

#include "interpreter.h"
#include "bytecode.h"
#include "token.h"

Instruction_t read_instruction(Interpreter_t* inter);
int exec_instr(Instruction_t instr, Interpreter_t* inter);
Literal_t read_number(Interpreter_t* inter);
int set_stack(Stack_t* stack, size_t index, Literal_t element);
Literal_t get_elem(Interpreter_t* inter, int64_t pos);
int exec_binary(
    Interpreter_t* inter, int flags, 
    Literal_t (*operation)(Literal_t, Literal_t) // higher order function
);
int read_flags(Interpreter_t* inter);
int extract_addressing_mode(int flags);
int extract_type(int flags);

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
Literal_t add_i(Literal_t n1, Literal_t n2) { Literal_t s; s.i = n1.i + n2.i; return s; }
Literal_t sub_i(Literal_t n1, Literal_t n2) { Literal_t s; s.i = n1.i - n2.i; return s; }
Literal_t mul_i(Literal_t n1, Literal_t n2) { Literal_t s; s.i = n1.i * n2.i; return s; }
Literal_t div_i(Literal_t n1, Literal_t n2) { Literal_t s; s.i = n1.i / n2.i; return s; }

Literal_t add_f(Literal_t n1, Literal_t n2) { Literal_t s; s.f = n1.f + n2.f; return s; }
Literal_t sub_f(Literal_t n1, Literal_t n2) { Literal_t s; s.f = n1.f - n2.f; return s; }
Literal_t mul_f(Literal_t n1, Literal_t n2) { Literal_t s; s.f = n1.f * n2.f; return s; }
Literal_t div_f(Literal_t n1, Literal_t n2) { Literal_t s; s.f = n1.f / n2.f; return s; }

int exec_instr(Instruction_t instr, Interpreter_t* inter) {
    switch (instr) {
        case Instruction_Add: {
            const int flags = read_flags(inter);
            int res = 0;
            switch (extract_type(flags)) {
                case TYPE_INT: { res = exec_binary(inter, flags, add_i); break; }
                case TYPE_FLOAT: { res = exec_binary(inter, flags, add_f); break; }
                default: { printf("Wrong type for instruction: %d\n", extract_type(flags)); res = 1; };
            }
            if (res == 1) { return res; }
            break;
        }
        case Instruction_Div: {
            const int flags = read_flags(inter);
            int res = 0;
            switch (extract_type(flags)) {
                case TYPE_INT: { res = exec_binary(inter, flags, div_i); break; }
                case TYPE_FLOAT: { res = exec_binary(inter, flags, div_f); break; }
                default: { printf("Wrong type for instruction: %d\n", extract_type(flags)); res = 1; };
            }
            if (res == 1) { return res; }
            break;
        }
        case Instruction_Mov: {
            const int flags = read_flags(inter);
            Literal_t op1 = read_number(inter);
            Literal_t op2 = read_number(inter);

            Literal_t num;
            switch (extract_addressing_mode(flags)) {
                case ADDRESSING_MODE_DIRECT:   num = op2; break;
                case ADDRESSING_MODE_INDIRECT: num = get_elem(inter, op2.i); break;
                default: printf("(unreachable) unkown addressing mode!\n"); return 1; break;
            }
            int res = set_stack(&inter->stack, (size_t)op1.i, num);
            if (res == 1) { return res; }
            break;
        }
        case Instruction_Mul: {
            const int flags = read_flags(inter);
            int res = 0;
            switch (extract_type(flags)) {
                case TYPE_INT: { res = exec_binary(inter, flags, mul_i); break; }
                case TYPE_FLOAT: { res = exec_binary(inter, flags, mul_f); break; }
                default: { printf("Wrong type for instruction: %d\n", extract_type(flags)); res = 1; };
            }
            if (res == 1) { return res; }
            break;
        }
        case Instruction_Neg: {
            const int flags = read_flags(inter);
            Literal_t op1 = read_number(inter);
            Literal_t n;
            switch (extract_type(flags)) {
                case TYPE_INT: {
                    n.i = -get_elem(inter, op1.i).i;
                    break;
                }
                case TYPE_FLOAT: {
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
            const int flags = read_flags(inter);
            int res = 0;
            switch (extract_type(flags)) {
                case TYPE_INT: { res = exec_binary(inter, flags, sub_i); break; }
                case TYPE_FLOAT: { res = exec_binary(inter, flags, sub_f); break; }
                default: { printf("Wrong type for instruction: %d\n", extract_type(flags)); res = 1; };
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
    Interpreter_t* inter, int flags, 
    Literal_t (*operation)(Literal_t, Literal_t) // higher order function
) {
    Literal_t op1 = read_number(inter);
    Literal_t op2 = read_number(inter);

    Literal_t num;
    switch (extract_addressing_mode(flags)) {
        case ADDRESSING_MODE_DIRECT: num = operation(get_elem(inter, op1.i), op2); break;
        case ADDRESSING_MODE_INDIRECT: num = operation(get_elem(inter, op1.i), get_elem(inter, op2.i)); break;
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

int read_flags(Interpreter_t* inter) {
    int flags = 0;
    for (size_t i = 0; i < sizeof(int); i++) {
        flags = flags | (inter->code[inter->instr_ptr  + i] << (i * 8));
    }
    inter->instr_ptr += sizeof(int);
    return flags;
}

int extract_addressing_mode(int flags) {
    return flags & ADDRESSING_MODE_PART;
}

int extract_type(int flags) {
    return flags & TYPE_PART;
}
