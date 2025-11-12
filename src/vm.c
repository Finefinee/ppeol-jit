#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// VM 생성
VM* vm_create() {
    VM* vm = (VM*)malloc(sizeof(VM));
    vm->chunk = NULL;
    vm->ip = NULL;
    vm->stack_top = 0;
    vm->globals = environment_create(NULL);
    
    // 스택 초기화
    for (int i = 0; i < STACK_MAX; i++) {
        vm->stack[i] = NULL;
    }
    
    return vm;
}

// VM 해제
void vm_free(VM* vm) {
    if (!vm) return;
    
    // 스택 정리
    for (int i = 0; i < vm->stack_top; i++) {
        // value_free(vm->stack[i]);  // 메모리 관리 간소화
    }
    
    environment_free(vm->globals);
    free(vm);
}

// 스택에 푸시
void vm_push(VM* vm, Value* value) {
    if (vm->stack_top >= STACK_MAX) {
        fprintf(stderr, "Stack overflow!\n");
        exit(1);
    }
    vm->stack[vm->stack_top++] = value;
}

// 스택에서 팝
Value* vm_pop(VM* vm) {
    if (vm->stack_top <= 0) {
        fprintf(stderr, "Stack underflow!\n");
        exit(1);
    }
    return vm->stack[--vm->stack_top];
}

// 스택 peek (제거하지 않고 확인)
Value* vm_peek(VM* vm, int distance) {
    if (vm->stack_top - 1 - distance < 0) {
        return NULL;
    }
    return vm->stack[vm->stack_top - 1 - distance];
}

// 다음 명령어 읽기
static Instruction* read_instruction(VM* vm) {
    return vm->ip++;
}

// VM 실행
void vm_run(VM* vm, BytecodeChunk* chunk) {
    vm->chunk = chunk;
    vm->ip = chunk->instructions;
    
    int instruction_count = 0;
    int max_instructions = 10000; // 안전 장치
    
    while (1) {
        if (instruction_count++ > max_instructions) {
            fprintf(stderr, "Too many instructions executed! Possible infinite loop.\n");
            return;
        }
        
        // 범위 체크
        long offset = vm->ip - chunk->instructions;
        if (offset < 0 || offset >= chunk->count) {
            fprintf(stderr, "Instruction pointer out of bounds: %ld (max: %d)\n", offset, chunk->count);
            return;
        }
        
        Instruction* instr = read_instruction(vm);
        
        switch (instr->opcode) {
            case OP_LOAD_CONST: {
                int index = (int)instr->operand.int_operand;
                Value* constant = chunk->constants[index];
                vm_push(vm, value_copy(constant));
                break;
            }
            
            case OP_LOAD_TRUE:
                vm_push(vm, value_create_bool(1));
                break;
            
            case OP_LOAD_FALSE:
                vm_push(vm, value_create_bool(0));
                break;
            
            case OP_LOAD_NULL:
                vm_push(vm, value_create_null());
                break;
            
            case OP_ADD: {
                Value* right = vm_pop(vm);
                Value* left = vm_pop(vm);
                
                if (left->type == VAL_NUMBER && right->type == VAL_NUMBER) {
                    vm_push(vm, value_create_number(left->data.number + right->data.number));
                } else if (left->type == VAL_STRING && right->type == VAL_STRING) {
                    // 문자열 연결
                    int len = strlen(left->data.string) + strlen(right->data.string);
                    char* result = (char*)malloc(len + 1);
                    strcpy(result, left->data.string);
                    strcat(result, right->data.string);
                    vm_push(vm, value_create_string(result));
                    free(result);
                } else {
                    fprintf(stderr, "Type error in ADD\n");
                    exit(1);
                }
                break;
            }
            
            case OP_SUBTRACT: {
                Value* right = vm_pop(vm);
                Value* left = vm_pop(vm);
                
                if (left->type == VAL_NUMBER && right->type == VAL_NUMBER) {
                    vm_push(vm, value_create_number(left->data.number - right->data.number));
                } else {
                    fprintf(stderr, "Type error in SUBTRACT\n");
                    exit(1);
                }
                break;
            }
            
            case OP_MULTIPLY: {
                Value* right = vm_pop(vm);
                Value* left = vm_pop(vm);
                
                if (left->type == VAL_NUMBER && right->type == VAL_NUMBER) {
                    vm_push(vm, value_create_number(left->data.number * right->data.number));
                } else if (left->type == VAL_STRING && right->type == VAL_NUMBER) {
                    // 문자열 반복
                    int repeat = (int)right->data.number;
                    if (repeat < 0) repeat = 0;
                    int len = strlen(left->data.string) * repeat;
                    char* result = (char*)malloc(len + 1);
                    result[0] = '\0';
                    for (int i = 0; i < repeat; i++) {
                        strcat(result, left->data.string);
                    }
                    vm_push(vm, value_create_string(result));
                    free(result);
                } else {
                    fprintf(stderr, "Type error in MULTIPLY\n");
                    exit(1);
                }
                break;
            }
            
            case OP_DIVIDE: {
                Value* right = vm_pop(vm);
                Value* left = vm_pop(vm);
                
                if (left->type == VAL_NUMBER && right->type == VAL_NUMBER) {
                    if (right->data.number == 0) {
                        fprintf(stderr, "Division by zero\n");
                        exit(1);
                    }
                    vm_push(vm, value_create_number(left->data.number / right->data.number));
                } else {
                    fprintf(stderr, "Type error in DIVIDE\n");
                    exit(1);
                }
                break;
            }
            
            case OP_MODULO: {
                Value* right = vm_pop(vm);
                Value* left = vm_pop(vm);
                
                if (left->type == VAL_NUMBER && right->type == VAL_NUMBER) {
                    if (right->data.number == 0) {
                        fprintf(stderr, "Modulo by zero\n");
                        exit(1);
                    }
                    vm_push(vm, value_create_number(fmod(left->data.number, right->data.number)));
                } else {
                    fprintf(stderr, "Type error in MODULO\n");
                    exit(1);
                }
                break;
            }
            
            case OP_FLOOR_DIV: {
                Value* right = vm_pop(vm);
                Value* left = vm_pop(vm);
                
                if (left->type == VAL_NUMBER && right->type == VAL_NUMBER) {
                    if (right->data.number == 0) {
                        fprintf(stderr, "Floor division by zero\n");
                        exit(1);
                    }
                    vm_push(vm, value_create_number(floor(left->data.number / right->data.number)));
                } else {
                    fprintf(stderr, "Type error in FLOOR_DIV\n");
                    exit(1);
                }
                break;
            }
            
            case OP_NEGATE: {
                Value* value = vm_pop(vm);
                if (value->type == VAL_NUMBER) {
                    vm_push(vm, value_create_number(-value->data.number));
                } else {
                    fprintf(stderr, "Type error in NEGATE\n");
                    exit(1);
                }
                break;
            }
            
            case OP_EQUAL: {
                Value* right = vm_pop(vm);
                Value* left = vm_pop(vm);
                
                int result = 0;
                if (left->type == VAL_NUMBER && right->type == VAL_NUMBER) {
                    result = (left->data.number == right->data.number);
                } else if (left->type == VAL_BOOL && right->type == VAL_BOOL) {
                    result = (left->data.boolean == right->data.boolean);
                } else if (left->type == VAL_STRING && right->type == VAL_STRING) {
                    result = (strcmp(left->data.string, right->data.string) == 0);
                }
                
                vm_push(vm, value_create_bool(result));
                break;
            }
            
            case OP_NOT_EQUAL: {
                Value* right = vm_pop(vm);
                Value* left = vm_pop(vm);
                
                int result = 1;
                if (left->type == VAL_NUMBER && right->type == VAL_NUMBER) {
                    result = (left->data.number != right->data.number);
                } else if (left->type == VAL_BOOL && right->type == VAL_BOOL) {
                    result = (left->data.boolean != right->data.boolean);
                } else if (left->type == VAL_STRING && right->type == VAL_STRING) {
                    result = (strcmp(left->data.string, right->data.string) != 0);
                }
                
                vm_push(vm, value_create_bool(result));
                break;
            }
            
            case OP_LESS: {
                Value* right = vm_pop(vm);
                Value* left = vm_pop(vm);
                
                if (left->type == VAL_NUMBER && right->type == VAL_NUMBER) {
                    vm_push(vm, value_create_bool(left->data.number < right->data.number));
                } else {
                    fprintf(stderr, "Type error in LESS\n");
                    exit(1);
                }
                break;
            }
            
            case OP_LESS_EQUAL: {
                Value* right = vm_pop(vm);
                Value* left = vm_pop(vm);
                
                if (left->type == VAL_NUMBER && right->type == VAL_NUMBER) {
                    vm_push(vm, value_create_bool(left->data.number <= right->data.number));
                } else {
                    fprintf(stderr, "Type error in LESS_EQUAL\n");
                    exit(1);
                }
                break;
            }
            
            case OP_GREATER: {
                Value* right = vm_pop(vm);
                Value* left = vm_pop(vm);
                
                if (left->type == VAL_NUMBER && right->type == VAL_NUMBER) {
                    vm_push(vm, value_create_bool(left->data.number > right->data.number));
                } else {
                    fprintf(stderr, "Type error in GREATER\n");
                    exit(1);
                }
                break;
            }
            
            case OP_GREATER_EQUAL: {
                Value* right = vm_pop(vm);
                Value* left = vm_pop(vm);
                
                if (left->type == VAL_NUMBER && right->type == VAL_NUMBER) {
                    vm_push(vm, value_create_bool(left->data.number >= right->data.number));
                } else {
                    fprintf(stderr, "Type error in GREATER_EQUAL\n");
                    exit(1);
                }
                break;
            }
            
            case OP_NOT: {
                Value* value = vm_pop(vm);
                int result = 0;
                
                if (value->type == VAL_BOOL) {
                    result = !value->data.boolean;
                } else if (value->type == VAL_NUMBER) {
                    result = (value->data.number == 0);
                }
                
                vm_push(vm, value_create_bool(result));
                break;
            }
            
            case OP_PRINT: {
                Value* value = vm_pop(vm);
                value_print(value);
                printf("\n");
                break;
            }
            
            case OP_POP: {
                vm_pop(vm);
                break;
            }
            
            case OP_DUP: {
                Value* value = vm_peek(vm, 0);
                vm_push(vm, value_copy(value));
                break;
            }
            
            case OP_LOAD_VAR: {
                int index = (int)instr->operand.int_operand;
                Value* var_name = chunk->constants[index];
                
                if (var_name->type != VAL_STRING) {
                    fprintf(stderr, "Variable name must be a string\n");
                    exit(1);
                }
                
                Value* value = environment_get(vm->globals, var_name->data.string);
                if (!value) {
                    fprintf(stderr, "Undefined variable: %s\n", var_name->data.string);
                    exit(1);
                }
                
                vm_push(vm, value_copy(value));
                break;
            }
            
            case OP_STORE_VAR: {
                int index = (int)instr->operand.int_operand;
                Value* var_name = chunk->constants[index];
                
                if (var_name->type != VAL_STRING) {
                    fprintf(stderr, "Variable name must be a string\n");
                    exit(1);
                }
                
                Value* value = vm_peek(vm, 0);
                environment_set(vm->globals, var_name->data.string, value);
                break;
            }
            
            case OP_BUILD_ARRAY: {
                int size = (int)instr->operand.int_operand;
                Value** elements = (Value**)malloc(sizeof(Value*) * size);
                
                // 스택에서 요소들을 역순으로 팝
                for (int i = size - 1; i >= 0; i--) {
                    elements[i] = vm_pop(vm);
                }
                
                Value* array = value_create_array(elements, size);
                
                // elements는 배열이 소유하므로 free하지 않음!
                vm_push(vm, array);
                break;
            }
            
            case OP_INDEX: {
                Value* index = vm_pop(vm);
                Value* array = vm_pop(vm);
                
                if (array->type == VAL_ARRAY) {
                    if (index->type != VAL_NUMBER) {
                        fprintf(stderr, "Array index must be a number\n");
                        exit(1);
                    }
                    
                    int idx = (int)index->data.number;
                    if (idx < 0 || idx >= array->data.array.count) {
                        fprintf(stderr, "Array index out of bounds: %d\n", idx);
                        exit(1);
                    }
                    
                    vm_push(vm, value_copy(array->data.array.elements[idx]));
                } else if (array->type == VAL_STRING) {
                    if (index->type != VAL_NUMBER) {
                        fprintf(stderr, "String index must be a number\n");
                        exit(1);
                    }
                    
                    int idx = (int)index->data.number;
                    int len = strlen(array->data.string);
                    if (idx < 0 || idx >= len) {
                        fprintf(stderr, "String index out of bounds: %d\n", idx);
                        exit(1);
                    }
                    
                    char str[2] = {array->data.string[idx], '\0'};
                    vm_push(vm, value_create_string(str));
                } else {
                    fprintf(stderr, "Cannot index non-array/string type\n");
                    exit(1);
                }
                break;
            }
            
            case OP_ARRAY_LENGTH: {
                Value* array = vm_pop(vm);
                
                if (array->type == VAL_ARRAY) {
                    vm_push(vm, value_create_number(array->data.array.count));
                } else if (array->type == VAL_STRING) {
                    vm_push(vm, value_create_number(strlen(array->data.string)));
                } else {
                    fprintf(stderr, "Cannot get length of non-array/string type\n");
                    exit(1);
                }
                break;
            }
            
            case OP_JUMP: {
                int target = (int)instr->operand.int_operand;
                vm->ip = &chunk->instructions[target];
                break;
            }
            
            case OP_JUMP_IF_FALSE: {
                Value* condition = vm_pop(vm);
                int is_false = 0;
                
                if (condition->type == VAL_BOOL) {
                    is_false = !condition->data.boolean;
                } else if (condition->type == VAL_NUMBER) {
                    is_false = (condition->data.number == 0);
                } else {
                    is_false = 0;
                }
                
                if (is_false) {
                    int target = (int)instr->operand.int_operand;
                    vm->ip = &chunk->instructions[target];
                }
                break;
            }
            
            case OP_HALT:
                return;
            
            default:
                fprintf(stderr, "Unknown opcode: %d\n", instr->opcode);
                exit(1);
        }
    }
}
