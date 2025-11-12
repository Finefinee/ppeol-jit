#ifndef VM_H
#define VM_H

#include "bytecode.h"
#include "interpreter.h"

#define STACK_MAX 256

// 가상 머신 구조체
typedef struct {
    BytecodeChunk* chunk;
    Instruction* ip;  // Instruction Pointer (현재 실행 중인 명령어)
    
    // 스택
    Value* stack[STACK_MAX];
    int stack_top;
    
    // 전역 변수
    Environment* globals;
} VM;

// VM 생성/해제
VM* vm_create();
void vm_free(VM* vm);

// VM 실행
void vm_run(VM* vm, BytecodeChunk* chunk);

// 스택 연산
void vm_push(VM* vm, Value* value);
Value* vm_pop(VM* vm);
Value* vm_peek(VM* vm, int distance);

#endif
