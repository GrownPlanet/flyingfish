#include <stdio.h>

#include "interpreter.h"
#include "bytecode.h"
#include "token.h"
#include "numtypes.h"

Instruction_t read_instruction(Interpreter_t* inter);
int exec_instr(Instruction_t instr, Interpreter_t* inter);
Literal_t read_number(Interpreter_t* inter);
int set_stack(Stack_t* stack, size_t index, Literal_t element);
Literal_t get_elem(Interpreter_t* inter, int_t pos);
int exec_binary(
    Interpreter_t* inter, int16_t flags,
    Literal_t (*operation)(Literal_t, Literal_t) // higher order function
);
int16_t read_flags(Interpreter_t* inter);
int16_t read_byte(Interpreter_t* inter);
int16_t extract_addressing_mode(int16_t flags);
int16_t extract_type(int16_t flags);

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
    printf("output:\n");
    while (interpreter.instr_ptr < interpreter.len) {
        Instruction_t instr = read_instruction(&interpreter);
        int res = exec_instr(instr, &interpreter);
        if (res == 1) { return res; }
    }

    printf("\nstack:\n");
    for (size_t i = 0; i < interpreter.stack.capacity; i++) {
        Literal_t sd = interpreter.stack.data[i];
        printf("  %" PRIx "; %f; %" PRId ",\n", sd.i, sd.f, sd.i);
    }

    return 0;
}

// arguments for the exec_binary function
Literal_t add_i(Literal_t n1, Literal_t n2) { Literal_t s; s.i = n1.i + n2.i; return s; }
Literal_t add_f(Literal_t n1, Literal_t n2) { Literal_t s; s.f = n1.f + n2.f; return s; }

Literal_t sub_i(Literal_t n1, Literal_t n2) { Literal_t s; s.i = n1.i - n2.i; return s; }
Literal_t sub_f(Literal_t n1, Literal_t n2) { Literal_t s; s.f = n1.f - n2.f; return s; }

Literal_t mul_i(Literal_t n1, Literal_t n2) { Literal_t s; s.i = n1.i * n2.i; return s; }
Literal_t mul_f(Literal_t n1, Literal_t n2) { Literal_t s; s.f = n1.f * n2.f; return s; }
 
Literal_t div_i(Literal_t n1, Literal_t n2) { Literal_t s; s.i = n1.i / n2.i; return s; }
Literal_t div_f(Literal_t n1, Literal_t n2) { Literal_t s; s.f = n1.f / n2.f; return s; }

Literal_t eqt_i(Literal_t n1, Literal_t n2) { Literal_t s; s.i = (n1.i == n2.i); return s; }
Literal_t eqt_f(Literal_t n1, Literal_t n2) { Literal_t s; s.f = (n1.f == n2.f); return s; }

Literal_t nqt_i(Literal_t n1, Literal_t n2) { Literal_t s; s.i = (n1.i != n2.i); return s; }
Literal_t nqt_f(Literal_t n1, Literal_t n2) { Literal_t s; s.f = (n1.f != n2.f); return s; }

Literal_t let_i(Literal_t n1, Literal_t n2) { Literal_t s; s.i = (n1.i < n2.i); return s; }
Literal_t let_f(Literal_t n1, Literal_t n2) { Literal_t s; s.f = (n1.f < n2.f); return s; }

Literal_t grt_i(Literal_t n1, Literal_t n2) { Literal_t s; s.i = (n1.i > n2.i); return s; }
Literal_t grt_f(Literal_t n1, Literal_t n2) { Literal_t s; s.f = (n1.f > n2.f); return s; }

Literal_t lqt_i(Literal_t n1, Literal_t n2) { Literal_t s; s.i = (n1.i <= n2.i); return s; }
Literal_t lqt_f(Literal_t n1, Literal_t n2) { Literal_t s; s.f = (n1.f <= n2.f); return s; }

Literal_t gqt_i(Literal_t n1, Literal_t n2) { Literal_t s; s.i = (n1.i >= n2.i); return s; }
Literal_t gqt_f(Literal_t n1, Literal_t n2) { Literal_t s; s.f = (n1.f >= n2.f); return s; }

#define BIN(f_i, f_f)\
    const int16_t flags = read_flags(inter);\
    int res = 0;\
    switch (extract_type(flags)) {\
        case TYPE_INT: { res |= exec_binary(inter, flags, f_i); break; }\
        case TYPE_FLOAT: { res |= exec_binary(inter, flags, f_f); break; }\
        default: { printf("Wrong type for instruction: %d\n", extract_type(flags)); res |= 1; };\
    }\
    if (res == 1) { return res; }\
    break;


int exec_instr(Instruction_t instr, Interpreter_t* inter) {
    switch (instr) {
        case Instruction_Mov: {
            const int16_t flags = read_flags(inter);
            Literal_t op1 = read_number(inter);
            Literal_t op2 = read_number(inter);

            Literal_t num;
            switch (extract_addressing_mode(flags)) {
                case ADDRESSING_MODE_DIRECT: num = op2; break;
                case ADDRESSING_MODE_INDIRECT: num = get_elem(inter, op2.i); break;
                default: printf("(unreachable) unknown addressing mode!\n"); return 1; break;
            }
            int res = set_stack(&inter->stack, op1.u, num);
            if (res == 1) { return res; }
            break;
        }
        case Instruction_Movs: {
            /* const int16_t flags = */ read_flags(inter);
            Literal_t op1 = read_number(inter);
            size_t len = read_number(inter).u;

            char* chs = (char*)malloc(sizeof(char) * len);
            if (chs == NULL) {
                printf("Malloc failed!\n");
                return 1;
            }

            for (size_t i = 0; i < len; i++) {
                chs[i] = read_byte(inter);
            }

            String_t* s = (String_t*)malloc(sizeof(String_t));
            if (s == NULL) {
                printf("Malloc failed!\n");
                return 1;
            }
            s->len = len;
            s->chars = chs;            

            Literal_t str;
            str.s = s;

            int res = set_stack(&inter->stack, op1.u, str);
            if (res == 1) { return res; }
            break;
        }
        case Instruction_Neg: {
            const int16_t flags = read_flags(inter);
            Literal_t op1 = read_number(inter);
            Literal_t n;
            int res = 0;
            switch (extract_type(flags)) {
                case TYPE_INT: { n.i = -get_elem(inter, op1.i).i; break; }
                case TYPE_FLOAT: { n.f = -get_elem(inter, op1.f).f; break; }
                default: {
                    printf("Wrong type for instruction Neg: %d\n", extract_type(flags));
                    res = 1;
                    n.i = 0;
                }
            }
            res |= set_stack(&inter->stack, op1.u, n);
            if (res == 1) { return res; }
            break;
        }
        case Instruction_Pri: {
            const int16_t flags = read_flags(inter);
            Literal_t op1 = read_number(inter);
            Literal_t num;

            switch (extract_addressing_mode(flags)) {
                case ADDRESSING_MODE_DIRECT: num = op1; break;
                case ADDRESSING_MODE_INDIRECT: num = get_elem(inter, op1.i); break;
                default: printf("(unreachable) unknown addressing mode!\n"); return 1; break;
            }

            switch (extract_type(flags)) {
                case TYPE_INT: printf("%" PRId "\n", num.i); break;
                case TYPE_FLOAT: printf("%f\n", num.f); break;
                case TYPE_BOOL: printf("%s\n", num.b ? "true" : "false"); break;
                case TYPE_CHAR: printf("%c\n", num.c); break;
                case TYPE_STRING: {
                    for (size_t i = 0; i < num.s->len; i++) {
                        printf("%c", num.s->chars[i]);
                    }
                    printf("\n");
                    break;
                }
                default: {
                    printf("Wrong type for instruction Pri: %d\n", extract_type(flags)); return 1;
                }
            }
            break;
        }
        case Instruction_Add: { BIN(add_i, add_f) }
        case Instruction_Div: { BIN(div_i, div_f) }
        case Instruction_Mul: { BIN(mul_i, mul_f) }
        case Instruction_Sub: { BIN(sub_i, sub_f) }
        case Instruction_Eqt: { BIN(eqt_i, eqt_f) } 
        case Instruction_Nqt: { BIN(nqt_i, nqt_f) } 
        case Instruction_Let: { BIN(let_i, let_f) } 
        case Instruction_Grt: { BIN(grt_i, grt_f) } 
        case Instruction_Lqt: { BIN(lqt_i, lqt_f) } 
        case Instruction_Gqt: { BIN(gqt_i, gqt_f) } 
        default:
            printf("Unknown instruction: %d not programmed yet!\n", instr);
            return 1;
            break;
    }
    return 0;
}

int exec_binary(
    Interpreter_t* inter, int16_t flags, 
    Literal_t (*operation)(Literal_t, Literal_t) // higher order function
) {
    Literal_t op1 = read_number(inter);
    Literal_t op2 = read_number(inter);

    Literal_t num;
    switch (extract_addressing_mode(flags)) {
        case ADDRESSING_MODE_DIRECT: 
            num = operation(get_elem(inter, op1.i), op2); break;
        case ADDRESSING_MODE_INDIRECT: 
            num = operation(get_elem(inter, op1.i), get_elem(inter, op2.i)); break;
        default: printf("(unreachable) unknown addressing mode!\n"); return 1; break;
    }
    int res = set_stack(&inter->stack, (size_t)op1.i, num);
    return res;
}

Literal_t read_number(Interpreter_t* inter) {
    int_t num = 0;
    for (size_t i = 0; i < sizeof(Literal_t); i++) {
        num = num | ((int_t)inter->code[inter->instr_ptr + i] << (i * 8));
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

Literal_t get_elem(Interpreter_t* inter, int_t pos) {
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

int16_t read_flags(Interpreter_t* inter) {
    int16_t flags = 0;
    for (size_t i = 0; i < sizeof(int16_t); i++) {
        flags = flags | (inter->code[inter->instr_ptr  + i] << (i * 8));
    }
    inter->instr_ptr += sizeof(int16_t);
    return flags;
}

int16_t read_byte(Interpreter_t* inter) {
    char byte = inter->code[inter->instr_ptr];
    inter->instr_ptr += sizeof(char);
    return byte;
}

int16_t extract_addressing_mode(int16_t flags) {
    return flags & ADDRESSING_MODE_PART;
}

int16_t extract_type(int16_t flags) {
    return flags & TYPE_PART;
}
