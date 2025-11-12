#ifndef BYTECODE_H
#define BYTECODE_H

#include <stdint.h>
#include "interpreter.h"

// 바이트코드 명령어 (OpCode)
typedef enum {
    // 상수 로드
    OP_LOAD_CONST,      // 상수를 스택에 푸시
    OP_LOAD_TRUE,       // true를 스택에 푸시
    OP_LOAD_FALSE,      // false를 스택에 푸시
    OP_LOAD_NULL,       // null을 스택에 푸시
    
    // 변수 연산
    OP_LOAD_VAR,        // 변수 로드
    OP_STORE_VAR,       // 변수 저장
    OP_LOAD_GLOBAL,     // 전역 변수 로드
    OP_STORE_GLOBAL,    // 전역 변수 저장
    
    // 산술 연산
    OP_ADD,             // +
    OP_SUBTRACT,        // -
    OP_MULTIPLY,        // *
    OP_DIVIDE,          // /
    OP_MODULO,          // %
    OP_FLOOR_DIV,       // //
    OP_NEGATE,          // 단항 -
    
    // 비교 연산
    OP_EQUAL,           // ==
    OP_NOT_EQUAL,       // !=
    OP_LESS,            // <
    OP_LESS_EQUAL,      // <=
    OP_GREATER,         // >
    OP_GREATER_EQUAL,   // >=
    
    // 논리 연산
    OP_NOT,             // !
    
    // 배열/딕셔너리
    OP_BUILD_ARRAY,     // 배열 생성 (피연산자: 요소 개수)
    OP_BUILD_DICT,      // 딕셔너리 생성 (피연산자: 키-값 쌍 개수)
    OP_INDEX,           // 인덱스 접근 []
    OP_STORE_INDEX,     // 인덱스에 저장
    OP_ARRAY_LENGTH,    // 배열 길이 가져오기
    
    // 제어 흐름
    OP_JUMP,            // 무조건 점프
    OP_JUMP_IF_FALSE,   // false면 점프
    OP_JUMP_IF_TRUE,    // true면 점프
    OP_LOOP,            // 루프 (뒤로 점프)
    
    // 함수
    OP_CALL,            // 함수 호출 (피연산자: 인자 개수)
    OP_RETURN,          // 함수 반환
    
    // 내장 함수
    OP_PRINT,           // print 함수
    
    // 스택 관리
    OP_POP,             // 스택에서 제거
    OP_DUP,             // 스택 top 복제
    
    // 프로그램 종료
    OP_HALT             // 프로그램 종료
} OpCode;

// 바이트코드 명령어 구조체
typedef struct {
    OpCode opcode;
    union {
        int64_t int_operand;      // 정수 피연산자
        double double_operand;    // 실수 피연산자
        char* string_operand;     // 문자열 피연산자
        void* ptr_operand;        // 포인터 피연산자
    } operand;
} Instruction;

// 바이트코드 청크 (명령어 모음)
typedef struct {
    Instruction* instructions;
    int count;
    int capacity;
    
    // 상수 풀
    Value** constants;
    int constant_count;
    int constant_capacity;
} BytecodeChunk;

// 바이트코드 함수
BytecodeChunk* bytecode_chunk_create();
void bytecode_chunk_free(BytecodeChunk* chunk);
void bytecode_emit(BytecodeChunk* chunk, OpCode opcode);
void bytecode_emit_with_operand(BytecodeChunk* chunk, OpCode opcode, int64_t operand);
int bytecode_add_constant(BytecodeChunk* chunk, Value* value);
void bytecode_disassemble(BytecodeChunk* chunk, const char* name);

#endif
