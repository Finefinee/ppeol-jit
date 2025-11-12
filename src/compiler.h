#ifndef COMPILER_H
#define COMPILER_H

#include "bytecode.h"
#include "parser.h"
#include "interpreter.h"

// 컴파일러 구조체
typedef struct {
    BytecodeChunk* chunk;
    Environment* env;
    int loop_start;     // 루프 시작 위치
    int loop_depth;     // 중첩 루프 깊이
} Compiler;

// 컴파일러 함수
Compiler* compiler_create();
void compiler_free(Compiler* compiler);

// AST → 바이트코드 컴파일
BytecodeChunk* compile(ASTNode* node);
void compile_statement(Compiler* compiler, ASTNode* node);
void compile_expression(Compiler* compiler, ASTNode* node);

// 최적화 (선택적)
void optimize_chunk(BytecodeChunk* chunk);

#endif
