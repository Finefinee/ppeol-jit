#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 컴파일러 생성
Compiler* compiler_create() {
    Compiler* compiler = (Compiler*)malloc(sizeof(Compiler));
    compiler->chunk = bytecode_chunk_create();
    compiler->env = environment_create(NULL);
    compiler->loop_start = -1;
    compiler->loop_depth = 0;
    return compiler;
}

// 컴파일러 해제
void compiler_free(Compiler* compiler) {
    if (!compiler) return;
    environment_free(compiler->env);
    free(compiler);
}

// 전방 선언
void compile_statement(Compiler* compiler, ASTNode* node);

// 표현식 컴파일
void compile_expression(Compiler* compiler, ASTNode* node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_NUMBER: {
            Value* value = value_create_number(node->data.number);
            int index = bytecode_add_constant(compiler->chunk, value);
            bytecode_emit_with_operand(compiler->chunk, OP_LOAD_CONST, index);
            break;
        }
        
        case AST_STRING: {
            Value* value = value_create_string(node->data.string);
            int index = bytecode_add_constant(compiler->chunk, value);
            bytecode_emit_with_operand(compiler->chunk, OP_LOAD_CONST, index);
            break;
        }
        
        case AST_BOOL: {
            if (node->data.boolean) {
                bytecode_emit(compiler->chunk, OP_LOAD_TRUE);
            } else {
                bytecode_emit(compiler->chunk, OP_LOAD_FALSE);
            }
            break;
        }
        
        case AST_IDENTIFIER: {
            // 변수 로드
            Value* value = value_create_string(node->data.string);
            int index = bytecode_add_constant(compiler->chunk, value);
            bytecode_emit_with_operand(compiler->chunk, OP_LOAD_VAR, index);
            break;
        }
        
        case AST_BINARY_OP: {
            // 왼쪽/오른쪽 컴파일
            compile_expression(compiler, node->data.binary.left);
            compile_expression(compiler, node->data.binary.right);
            
            // 연산자 선택
            char* op = node->data.binary.op;
            if (strcmp(op, "+") == 0) bytecode_emit(compiler->chunk, OP_ADD);
            else if (strcmp(op, "-") == 0) bytecode_emit(compiler->chunk, OP_SUBTRACT);
            else if (strcmp(op, "*") == 0) bytecode_emit(compiler->chunk, OP_MULTIPLY);
            else if (strcmp(op, "/") == 0) bytecode_emit(compiler->chunk, OP_DIVIDE);
            else if (strcmp(op, "%") == 0) bytecode_emit(compiler->chunk, OP_MODULO);
            else if (strcmp(op, "==") == 0) bytecode_emit(compiler->chunk, OP_EQUAL);
            else if (strcmp(op, "!=") == 0) bytecode_emit(compiler->chunk, OP_NOT_EQUAL);
            else if (strcmp(op, "<") == 0) bytecode_emit(compiler->chunk, OP_LESS);
            else if (strcmp(op, "<=") == 0) bytecode_emit(compiler->chunk, OP_LESS_EQUAL);
            else if (strcmp(op, ">") == 0) bytecode_emit(compiler->chunk, OP_GREATER);
            else if (strcmp(op, ">=") == 0) bytecode_emit(compiler->chunk, OP_GREATER_EQUAL);
            break;
        }
        
        case AST_UNARY_OP: {
            compile_expression(compiler, node->data.unary.operand);
            
            char* op = node->data.unary.op;
            if (strcmp(op, "-") == 0) bytecode_emit(compiler->chunk, OP_NEGATE);
            else if (strcmp(op, "!") == 0) bytecode_emit(compiler->chunk, OP_NOT);
            break;
        }
        
        case AST_FUNCTION_CALL: {
            // print 함수 특별 처리
            if (strcmp(node->data.function_call.name, "print") == 0) {
                for (int i = 0; i < node->data.function_call.arg_count; i++) {
                    compile_expression(compiler, node->data.function_call.args[i]);
                }
                // 인자 개수만큼 PRINT (역순으로 팝되므로)
                for (int i = 0; i < node->data.function_call.arg_count; i++) {
                    bytecode_emit(compiler->chunk, OP_PRINT);
                }
                // print는 값을 반환하지 않으므로 null 푸시
                bytecode_emit(compiler->chunk, OP_LOAD_NULL);
            }
            break;
        }
        
        case AST_ARRAY: {
            for (int i = 0; i < node->data.array.element_count; i++) {
                compile_expression(compiler, node->data.array.elements[i]);
            }
            bytecode_emit_with_operand(compiler->chunk, OP_BUILD_ARRAY, node->data.array.element_count);
            break;
        }
        
        case AST_INDEX: {
            compile_expression(compiler, node->data.index.array);
            compile_expression(compiler, node->data.index.index);
            bytecode_emit(compiler->chunk, OP_INDEX);
            break;
        }
        
        default:
            // 지원하지 않는 표현식은 무시
            break;
    }
}

// 문장 컴파일
void compile_statement(Compiler* compiler, ASTNode* node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_LET: {
            // let x = 값
            if (node->data.assign.value) {
                compile_expression(compiler, node->data.assign.value);
            } else {
                bytecode_emit(compiler->chunk, OP_LOAD_NULL);
            }
            
            Value* name = value_create_string(node->data.assign.name);
            int index = bytecode_add_constant(compiler->chunk, name);
            bytecode_emit_with_operand(compiler->chunk, OP_STORE_VAR, index);
            break;
        }
        
        case AST_ASSIGN: {
            // x = 값
            compile_expression(compiler, node->data.assign.value);
            
            Value* name = value_create_string(node->data.assign.name);
            int index = bytecode_add_constant(compiler->chunk, name);
            bytecode_emit_with_operand(compiler->chunk, OP_STORE_VAR, index);
            break;
        }
        
        case AST_IF: {
            // 조건식
            compile_expression(compiler, node->data.if_stmt.condition);
            
            // JUMP_IF_FALSE
            int jump_to_else = compiler->chunk->count;
            bytecode_emit_with_operand(compiler->chunk, OP_JUMP_IF_FALSE, 0);
            
            // then 블록
            bytecode_emit(compiler->chunk, OP_POP);
            compile_statement(compiler, node->data.if_stmt.then_branch);
            
            // JUMP (else 건너뛰기)
            int jump_to_end = compiler->chunk->count;
            bytecode_emit_with_operand(compiler->chunk, OP_JUMP, 0);
            
            // else 시작
            compiler->chunk->instructions[jump_to_else].operand.int_operand = compiler->chunk->count;
            bytecode_emit(compiler->chunk, OP_POP);
            
            if (node->data.if_stmt.else_branch) {
                compile_statement(compiler, node->data.if_stmt.else_branch);
            }
            
            // 끝
            compiler->chunk->instructions[jump_to_end].operand.int_operand = compiler->chunk->count;
            break;
        }
        
        case AST_WHILE: {
            // 루프 시작
            int loop_start = compiler->chunk->count;
            
            // 조건식
            compile_expression(compiler, node->data.while_loop.condition);
            
            // JUMP_IF_FALSE
            int jump_to_end = compiler->chunk->count;
            bytecode_emit_with_operand(compiler->chunk, OP_JUMP_IF_FALSE, 0);
            
            // 본문
            bytecode_emit(compiler->chunk, OP_POP);
            compiler->loop_start = loop_start;
            compiler->loop_depth++;
            compile_statement(compiler, node->data.while_loop.body);
            compiler->loop_depth--;
            
            // 루프 시작으로
            bytecode_emit_with_operand(compiler->chunk, OP_JUMP, loop_start);
            
            // 끝
            compiler->chunk->instructions[jump_to_end].operand.int_operand = compiler->chunk->count;
            bytecode_emit(compiler->chunk, OP_POP);
            break;
        }
        
        case AST_FOR: {
            // for (item in iterable) 형태를 while 루프로 변환
            // 간단한 구현: 배열에 대한 반복만 지원
            
            // iterable 평가하고 임시 변수에 저장
            compile_expression(compiler, node->data.for_loop.iterable);
            Value* tmp_array = value_create_string("__tmp_array__");
            int array_idx = bytecode_add_constant(compiler->chunk, tmp_array);
            bytecode_emit_with_operand(compiler->chunk, OP_STORE_VAR, array_idx);
            bytecode_emit(compiler->chunk, OP_POP);  // 스택에서 값 제거
            
            // 인덱스 = 0
            Value* tmp_index = value_create_string("__tmp_idx__");
            int idx_var = bytecode_add_constant(compiler->chunk, tmp_index);
            int zero_const = bytecode_add_constant(compiler->chunk, value_create_number(0));
            bytecode_emit_with_operand(compiler->chunk, OP_LOAD_CONST, zero_const);
            bytecode_emit_with_operand(compiler->chunk, OP_STORE_VAR, idx_var);
            bytecode_emit(compiler->chunk, OP_POP);  // 스택에서 값 제거
            
            // 루프 시작
            int loop_start = compiler->chunk->count;
            
            // 조건: __tmp_idx__ < 배열.길이
            bytecode_emit_with_operand(compiler->chunk, OP_LOAD_VAR, idx_var);
            bytecode_emit_with_operand(compiler->chunk, OP_LOAD_VAR, array_idx);
            bytecode_emit(compiler->chunk, OP_ARRAY_LENGTH);
            bytecode_emit(compiler->chunk, OP_LESS);
            
            int jump_to_end = compiler->chunk->count;
            bytecode_emit_with_operand(compiler->chunk, OP_JUMP_IF_FALSE, 0);
            // POP 제거: JUMP_IF_FALSE가 이미 pop함
            
            // iterator = __tmp_array__[__tmp_idx__]
            bytecode_emit_with_operand(compiler->chunk, OP_LOAD_VAR, array_idx);
            bytecode_emit_with_operand(compiler->chunk, OP_LOAD_VAR, idx_var);
            bytecode_emit(compiler->chunk, OP_INDEX);
            
            Value* iter = value_create_string(node->data.for_loop.iterator);
            int iter_idx = bytecode_add_constant(compiler->chunk, iter);
            bytecode_emit_with_operand(compiler->chunk, OP_STORE_VAR, iter_idx);
            bytecode_emit(compiler->chunk, OP_POP);  // 스택에서 값 제거
            
            // 본문 실행
            compiler->loop_start = loop_start;
            compiler->loop_depth++;
            compile_statement(compiler, node->data.for_loop.body);
            compiler->loop_depth--;
            
            // __tmp_idx__ = __tmp_idx__ + 1
            bytecode_emit_with_operand(compiler->chunk, OP_LOAD_VAR, idx_var);
            int one_const = bytecode_add_constant(compiler->chunk, value_create_number(1));
            bytecode_emit_with_operand(compiler->chunk, OP_LOAD_CONST, one_const);
            bytecode_emit(compiler->chunk, OP_ADD);
            bytecode_emit_with_operand(compiler->chunk, OP_STORE_VAR, idx_var);
            bytecode_emit(compiler->chunk, OP_POP);  // 스택에서 값 제거
            
            // 루프 시작으로
            bytecode_emit_with_operand(compiler->chunk, OP_JUMP, loop_start);
            
            // 끝 (POP 불필요: JUMP_IF_FALSE가 이미 pop함)
            compiler->chunk->instructions[jump_to_end].operand.int_operand = compiler->chunk->count;
            break;
        }
        
        case AST_BLOCK: {
            for (int i = 0; i < node->data.block.statement_count; i++) {
                compile_statement(compiler, node->data.block.statements[i]);
            }
            break;
        }
        
        default:
            // 표현식으로 처리 (함수 호출은 제외)
            if (node->type != AST_FUNCTION_CALL) {
                compile_expression(compiler, node);
                bytecode_emit(compiler->chunk, OP_PRINT);
            } else {
                compile_expression(compiler, node);
                bytecode_emit(compiler->chunk, OP_POP);  // 반환값 버리기
            }
            break;
    }
}

// 메인 컴파일 함수
BytecodeChunk* compile(ASTNode* node) {
    Compiler* compiler = compiler_create();
    
    // 프로그램 컴파일 (AST_PROGRAM은 block 구조 사용)
    if (node->type == AST_PROGRAM) {
        for (int i = 0; i < node->data.block.statement_count; i++) {
            compile_statement(compiler, node->data.block.statements[i]);
        }
    } else {
        compile_statement(compiler, node);
    }
    
    // HALT 추가
    bytecode_emit(compiler->chunk, OP_HALT);
    
    BytecodeChunk* chunk = compiler->chunk;
    compiler->chunk = NULL;
    compiler_free(compiler);
    
    return chunk;
}

// 최적화 (나중에 구현)
void optimize_chunk(BytecodeChunk* chunk) {
    // TODO
}
