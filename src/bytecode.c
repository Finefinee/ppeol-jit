#include "bytecode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 바이트코드 청크 생성
BytecodeChunk* bytecode_chunk_create() {
    BytecodeChunk* chunk = (BytecodeChunk*)malloc(sizeof(BytecodeChunk));
    chunk->instructions = NULL;
    chunk->count = 0;
    chunk->capacity = 0;
    
    chunk->constants = NULL;
    chunk->constant_count = 0;
    chunk->constant_capacity = 0;
    
    return chunk;
}

// 바이트코드 청크 해제
void bytecode_chunk_free(BytecodeChunk* chunk) {
    if (!chunk) return;
    
    // 명령어 해제
    if (chunk->instructions) {
        free(chunk->instructions);
    }
    
    // 상수 해제
    for (int i = 0; i < chunk->constant_count; i++) {
        value_free(chunk->constants[i]);
    }
    if (chunk->constants) {
        free(chunk->constants);
    }
    
    free(chunk);
}

// 명령어 추가
void bytecode_emit(BytecodeChunk* chunk, OpCode opcode) {
    // 용량 확장 필요 시
    if (chunk->count >= chunk->capacity) {
        int old_capacity = chunk->capacity;
        chunk->capacity = old_capacity < 8 ? 8 : old_capacity * 2;
        chunk->instructions = (Instruction*)realloc(chunk->instructions, 
                                                    sizeof(Instruction) * chunk->capacity);
    }
    
    // 명령어 추가
    chunk->instructions[chunk->count].opcode = opcode;
    chunk->instructions[chunk->count].operand.int_operand = 0;
    chunk->count++;
}

// 피연산자가 있는 명령어 추가
void bytecode_emit_with_operand(BytecodeChunk* chunk, OpCode opcode, int64_t operand) {
    // 용량 확장 필요 시
    if (chunk->count >= chunk->capacity) {
        int old_capacity = chunk->capacity;
        chunk->capacity = old_capacity < 8 ? 8 : old_capacity * 2;
        chunk->instructions = (Instruction*)realloc(chunk->instructions, 
                                                    sizeof(Instruction) * chunk->capacity);
    }
    
    // 명령어 추가
    chunk->instructions[chunk->count].opcode = opcode;
    chunk->instructions[chunk->count].operand.int_operand = operand;
    chunk->count++;
}

// 상수 추가 (상수 풀에 추가하고 인덱스 반환)
int bytecode_add_constant(BytecodeChunk* chunk, Value* value) {
    // 용량 확장 필요 시
    if (chunk->constant_count >= chunk->constant_capacity) {
        int old_capacity = chunk->constant_capacity;
        chunk->constant_capacity = old_capacity < 8 ? 8 : old_capacity * 2;
        chunk->constants = (Value**)realloc(chunk->constants, 
                                           sizeof(Value*) * chunk->constant_capacity);
    }
    
    // 상수 추가
    chunk->constants[chunk->constant_count] = value;
    return chunk->constant_count++;
}

// OpCode를 문자열로 변환
static const char* opcode_to_string(OpCode opcode) {
    switch (opcode) {
        case OP_LOAD_CONST: return "LOAD_CONST";
        case OP_LOAD_TRUE: return "LOAD_TRUE";
        case OP_LOAD_FALSE: return "LOAD_FALSE";
        case OP_LOAD_NULL: return "LOAD_NULL";
        case OP_LOAD_VAR: return "LOAD_VAR";
        case OP_STORE_VAR: return "STORE_VAR";
        case OP_LOAD_GLOBAL: return "LOAD_GLOBAL";
        case OP_STORE_GLOBAL: return "STORE_GLOBAL";
        case OP_ADD: return "ADD";
        case OP_SUBTRACT: return "SUBTRACT";
        case OP_MULTIPLY: return "MULTIPLY";
        case OP_DIVIDE: return "DIVIDE";
        case OP_MODULO: return "MODULO";
        case OP_FLOOR_DIV: return "FLOOR_DIV";
        case OP_NEGATE: return "NEGATE";
        case OP_EQUAL: return "EQUAL";
        case OP_NOT_EQUAL: return "NOT_EQUAL";
        case OP_LESS: return "LESS";
        case OP_LESS_EQUAL: return "LESS_EQUAL";
        case OP_GREATER: return "GREATER";
        case OP_GREATER_EQUAL: return "GREATER_EQUAL";
        case OP_NOT: return "NOT";
        case OP_BUILD_ARRAY: return "BUILD_ARRAY";
        case OP_BUILD_DICT: return "BUILD_DICT";
        case OP_INDEX: return "INDEX";
        case OP_STORE_INDEX: return "STORE_INDEX";
        case OP_ARRAY_LENGTH: return "ARRAY_LENGTH";
        case OP_JUMP: return "JUMP";
        case OP_JUMP_IF_FALSE: return "JUMP_IF_FALSE";
        case OP_JUMP_IF_TRUE: return "JUMP_IF_TRUE";
        case OP_LOOP: return "LOOP";
        case OP_CALL: return "CALL";
        case OP_RETURN: return "RETURN";
        case OP_PRINT: return "PRINT";
        case OP_POP: return "POP";
        case OP_DUP: return "DUP";
        case OP_HALT: return "HALT";
        default: return "UNKNOWN";
    }
}

// 바이트코드 디스어셈블 (디버깅용)
void bytecode_disassemble(BytecodeChunk* chunk, const char* name) {
    printf("== %s ==\n", name);
    
    for (int i = 0; i < chunk->count; i++) {
        Instruction* instr = &chunk->instructions[i];
        printf("%04d  %-20s", i, opcode_to_string(instr->opcode));
        
        // 피연산자 출력
        switch (instr->opcode) {
            case OP_LOAD_CONST:
            case OP_JUMP:
            case OP_JUMP_IF_FALSE:
            case OP_JUMP_IF_TRUE:
            case OP_LOOP:
            case OP_CALL:
            case OP_BUILD_ARRAY:
            case OP_BUILD_DICT:
                printf(" %lld", instr->operand.int_operand);
                
                // 상수 풀 인덱스인 경우 값도 출력
                if (instr->opcode == OP_LOAD_CONST && 
                    instr->operand.int_operand < chunk->constant_count) {
                    printf(" (");
                    value_print(chunk->constants[instr->operand.int_operand]);
                    printf(")");
                }
                break;
            
            case OP_LOAD_VAR:
            case OP_STORE_VAR:
                printf(" %lld", instr->operand.int_operand);
                
                // 변수 이름 출력
                if (instr->operand.int_operand < chunk->constant_count) {
                    printf(" (");
                    value_print(chunk->constants[instr->operand.int_operand]);
                    printf(")");
                }
                break;
            
            default:
                break;
        }
        
        printf("\n");
    }
    
    printf("\n");
}
