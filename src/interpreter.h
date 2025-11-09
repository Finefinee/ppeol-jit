#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser.h"
#include <math.h>

// 전방 선언
struct StackFrame;
struct ModuleCache;

// 값 타입
typedef enum {
    VAL_NUMBER,
    VAL_STRING,
    VAL_ARRAY,
    VAL_DICT,
    VAL_FUNCTION,
    VAL_CLASS,
    VAL_INSTANCE,
    VAL_EXCEPTION,
    VAL_MODULE,
    VAL_MATRIX,
    VAL_NULL
} ValueType;

// 값 구조체
typedef struct Value {
    ValueType type;
    union {
        double number;
        char* string;
        struct {
            struct Value** elements;
            int count;
        } array;
        struct {
            char** keys;
            struct Value** values;
            int count;
        } dict;
        struct {
            char** params;
            int param_count;
            ASTNode* body;
            struct Environment* closure;
        } function;
        struct {
            char* name;
            char** fields;
            int field_count;
            ASTNode** methods;
            int method_count;
            char* parent_class;  // 부모 클래스 이름
        } class_def;
        struct {
            char* class_name;
            struct Value** field_values;
            int field_count;
            char** field_names;
            char* parent_class;  // 부모 클래스 이름
        } instance;
        struct {
            char* type;     // "TypeError", "ValueError" 등
            char* message;  // 에러 메시지
            struct StackFrame* stack_trace;  // 스택 트레이스
            int stack_depth;          // 스택 깊이
        } exception;
        struct {
            char* name;  // 모듈 이름
            struct Environment* exports;  // 모듈의 export된 심볼들
        } module;
        struct {
            double** data;  // 2차원 배열 (행렬 데이터)
            int rows;       // 행 개수
            int cols;       // 열 개수
        } matrix;
    } data;
} Value;

// 환경 (변수 스코프)
typedef struct Environment {
    char** names;
    Value** values;
    int count;
    int capacity;
    struct Environment* parent;
} Environment;

// 스택 프레임 (함수 호출 정보) - 전방 선언을 위해 typedef 분리
struct StackFrame {
    char* function_name;       // 함수 이름
    char* file_name;           // 파일 이름
    int line_number;           // 라인 번호
    struct StackFrame* next;   // 다음 프레임
};
typedef struct StackFrame StackFrame;

// 인터프리터 구조체
typedef struct {
    Environment* global_env;
    Environment* current_env;
    Value* return_value;
    int has_returned;
    Value* current_exception;  // 현재 발생한 예외
    int has_exception;         // 예외 발생 플래그
    StackFrame* call_stack;    // 함수 호출 스택
    int stack_depth;           // 현재 스택 깊이
    int max_stack_depth;       // 최대 스택 깊이 (기본: 1000)
    char* current_file;        // 현재 실행 중인 파일
    struct ModuleCache* module_cache;  // 모듈 캐시
} Interpreter;

// 함수 선언
Interpreter* interpreter_create();
void interpreter_free(Interpreter* interp);
Value* interpreter_eval(Interpreter* interp, ASTNode* node);
Value* interpreter_eval_binary(Interpreter* interp, ASTNode* node);
Value* interpreter_eval_function_call(Interpreter* interp, ASTNode* node);
Environment* environment_create(Environment* parent);
void environment_free(Environment* env);
void environment_set(Environment* env, char* name, Value* value);
Value* environment_get(Environment* env, char* name);
Value* value_create_number(double num);
Value* value_create_string(char* str);
Value* value_create_array(Value** elements, int count);
Value* value_create_dict(char** keys, Value** values, int count);
Value* value_create_exception(char* type, char* message);
Value* value_create_module(char* name, Environment* exports);
Value* value_create_matrix(int rows, int cols);
Value* value_create_null();
Value* value_copy(Value* val);
void value_free(Value* val);
void value_print(Value* val);

// 스택 프레임 함수
StackFrame* stack_frame_create(char* function_name, char* file_name, int line_number);
void stack_frame_free(StackFrame* frame);
Value* stack_push(Interpreter* interp, char* function_name, char* file_name, int line_number);
void stack_pop(Interpreter* interp);
StackFrame* stack_copy(StackFrame* stack);
void print_stack_trace(StackFrame* stack, int depth);
void exception_attach_stack_trace(Interpreter* interp, Value* exception);

#endif
