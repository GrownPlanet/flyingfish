#include <stdint.h>
#include <stdio.h>

#include "interpreter.h"
#include "bytecode.h"

Instruction_t read_instruction(Interpreter_t* inter);
int exec_instr(Instruction_t instr, Interpreter_t* inter);
int64_t read_number(Interpreter_t* inter);
AddressingMode_t read_addressing_mode(Interpreter_t* inter);
int set_stack(Stack_t* stack, size_t index, int64_t element);

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
    for (int i = 0; i < interpreter.stack.capacity; i++) {
        printf("  %ld,\n", interpreter.stack.data[i]);
    }

    return 0;
}

int exec_instr(Instruction_t instr, Interpreter_t* inter) {
    switch (instr) {
        case Instruction_Mov: {
            int64_t op1 = read_number(inter);
            AddressingMode_t adr2 = read_addressing_mode(inter);
            int64_t op2 = read_number(inter);

            switch (adr2) {
                case AddressingMode_Direct: {
                    int res = set_stack(&inter->stack, (size_t)op1, op2);
                    if (res == 1) { return res; }
                    break;
                }
                case AddressingMode_Indirect: {
                    int res = set_stack(&inter->stack, (size_t)op1, inter->stack.data[(size_t)op2]);
                    if (res == 1) { return res; }
                    break;
                }
            }
            break;
        }
        case Instruction_MulI: {
            int64_t op1 = read_number(inter);
            AddressingMode_t adr2 = read_addressing_mode(inter);
            int64_t op2 = read_number(inter);

            switch (adr2) {
                case AddressingMode_Direct: {
                    int64_t num = inter->stack.data[(size_t)op1] * op2;
                    int res = set_stack(&inter->stack, (size_t)op1, num);
                    if (res == 1) { return res; }
                    break;
                }
                case AddressingMode_Indirect: {
                    int64_t num = inter->stack.data[(size_t)op1] * inter->stack.data[(size_t)op2];
                    int res = set_stack(&inter->stack, (size_t)op1, num);
                    if (res == 1) { return res; }
                    break;
                }
            }
            break;
        }
        default:
            printf("TODO: instruction %d not programmed yet!\n", instr);
            return 1;
            break;
    }
}

int64_t read_number(Interpreter_t* inter) {
    int64_t num = 0;
    for (size_t i = 0; i < sizeof(int64_t); i++) {
        num = num | (inter->code[inter->instr_ptr  + i] << (i * 8));
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

int set_stack(Stack_t* stack, size_t index, int64_t element) {
    if (index >= stack->capacity) {
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
