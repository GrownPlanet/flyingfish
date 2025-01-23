#include <stdint.h>
#include <stdio.h>

#include "interpreter.h"
#include "bytecode.h"

Instruction_t read_instruction(Interpreter_t* inter);
int exec_instr(Instruction_t instr, Interpreter_t* inter);
int64_t read_number(Interpreter_t* inter);
AddressingMode_t read_addressing_mode(Interpreter_t* inter);
int set_stack(Stack_t* stack, size_t index, int64_t element);
int64_t get_elem(Interpreter_t* inter, int64_t pos);
int exec_binary(
    Interpreter_t* inter, int64_t (*operation)(int64_t, int64_t) // higher order function
);

Interpreter_t new_interpreter(unsigned char* code, size_t len) {
    Stack_t stack = {
        .data = (int64_t*)malloc(sizeof(int64_t)),
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
        printf("  %f,\n", (double)interpreter.stack.data[i]);
    }

    return 0;
}

// arguments for the exec_binary function
int64_t add_i(int64_t n1, int64_t n2) { return n1 + n2; }
int64_t sub_i(int64_t n1, int64_t n2) { return n1 - n2; }
int64_t mul_i(int64_t n1, int64_t n2) { return n1 * n2; }
int64_t div_i(int64_t n1, int64_t n2) { return n1 / n2; }

int exec_instr(Instruction_t instr, Interpreter_t* inter) {
    switch (instr) {
        case Instruction_AddI: {
            int res = exec_binary(inter, add_i);
            if (res == 1) { return res; }
            break;
        }
        case Instruction_DivI: {
            int res = exec_binary(inter, div_i);
            if (res == 1) { return res; }
            break;
        }
        case Instruction_Mov: {
            int64_t op1 = read_number(inter);
            AddressingMode_t adr2 = read_addressing_mode(inter);
            int64_t op2 = read_number(inter);

            int64_t num;
            switch (adr2) {
                case AddressingMode_Direct: num = op2; break;
                case AddressingMode_Indirect: num = get_elem(inter, op2); break;
                default: printf("(unreachable) unkown addressing mode!\n"); return 1; break;
            }
            int res = set_stack(&inter->stack, (size_t)op1, num);
            if (res == 1) { return res; }
            break;
        }
        case Instruction_MulI: {
            int res = exec_binary(inter, mul_i);
            if (res == 1) { return res; }
            break;
        }
        case Instruction_NegI: {
            int64_t op1 = read_number(inter);
            int res = set_stack(&inter->stack, (size_t)op1, -get_elem(inter, op1));
            if (res == 1) { return res; }
            break;
        }
        case Instruction_SubI: {
            int res = exec_binary(inter, sub_i);
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
    Interpreter_t* inter, int64_t (*operation)(int64_t, int64_t) // higher order function
) {
    int64_t op1 = read_number(inter);
    AddressingMode_t adr2 = read_addressing_mode(inter);
    int64_t op2 = read_number(inter);

    int64_t num;
    switch (adr2) {
        case AddressingMode_Direct: num = operation(get_elem(inter, op1), op2); break;
        case AddressingMode_Indirect: num = operation(get_elem(inter, op1), get_elem(inter, op2)); break;
        default: printf("(unreachable) unkown addressing mode!\n"); return 1; break;
    }
    int res = set_stack(&inter->stack, (size_t)op1, num);
    return res;
}

int64_t read_number(Interpreter_t* inter) {
    int64_t num = 0;
    for (size_t i = 0; i < sizeof(int64_t); i++) {
        num = num | ((int64_t)inter->code[inter->instr_ptr + i] << (i * 8));
    }
    inter->instr_ptr += sizeof(int64_t);
    return num;
}

AddressingMode_t read_addressing_mode(Interpreter_t* inter) {
    int adrm = 0;
    for (size_t i = 0; i < sizeof(AddressingMode_t); i++) {
        adrm = adrm | (inter->code[inter->instr_ptr  + i] << (i * 8));
    }
    inter->instr_ptr += sizeof(AddressingMode_t);
    return (AddressingMode_t)adrm;
}

Instruction_t read_instruction(Interpreter_t* inter) {
    int instruction = 0;
    for (size_t i = 0; i < sizeof(Instruction_t); i++) {
        instruction = instruction | (inter->code[inter->instr_ptr  + i] << (i * 8));
    }
    inter->instr_ptr += sizeof(Instruction_t);
    return (Instruction_t)instruction;
}

int64_t get_elem(Interpreter_t* inter, int64_t pos) {
    return inter->stack.data[(size_t)pos];
}

int set_stack(Stack_t* stack, size_t index, int64_t element) {
    while (index >= stack->capacity) {
        stack->capacity *= 2;
        stack->data = realloc(stack->data, stack->capacity * sizeof(int64_t));

        if (stack->data == NULL) {
            printf("Realloc failed!\n");
            return 1;
        }
    }

    stack->data[index] = element;
    return 0;
}
