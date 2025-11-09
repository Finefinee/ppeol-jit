#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

// AST 노드 타입
typedef enum {
    AST_PROGRAM,
    AST_NUMBER,
    AST_STRING,
    AST_IDENTIFIER,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_ASSIGN,
    AST_LET,
    AST_FUNCTION_DEF,
    AST_FUNCTION_CALL,
    AST_IF,
    AST_FOR,
    AST_WHILE,
    AST_RETURN,
    AST_BLOCK,
    AST_ARRAY,
    AST_DICT,
    AST_INDEX,
    AST_DOT_ACCESS,
    AST_CLASS,
    AST_NEW,
    AST_THIS,
    AST_METHOD_CALL,
    AST_FIELD_ASSIGN,
    AST_SUPER,
    AST_TRY_CATCH,
    AST_THROW,
    AST_ASSERT,
    AST_IMPORT,
    AST_EXPORT
} ASTNodeType;

// AST 노드 구조체
typedef struct ASTNode {
    ASTNodeType type;
    union {
        double number;
        char* string;
        struct {
            char* op;
            struct ASTNode* left;
            struct ASTNode* right;
        } binary;
        struct {
            char* op;
            struct ASTNode* operand;
        } unary;
        struct {
            char* name;
            struct ASTNode* value;
        } assign;
        struct {
            char* name;
            char** params;
            int param_count;
            struct ASTNode* body;
        } function_def;
        struct {
            char* name;
            struct ASTNode** args;
            int arg_count;
        } function_call;
        struct {
            struct ASTNode* condition;
            struct ASTNode* then_branch;
            struct ASTNode* else_branch;
        } if_stmt;
        struct {
            char* iterator;
            struct ASTNode* iterable;
            struct ASTNode* body;
            int is_parallel;
        } for_loop;
        struct {
            struct ASTNode* condition;
            struct ASTNode* body;
        } while_loop;
        struct {
            struct ASTNode* value;
        } return_stmt;
        struct {
            struct ASTNode** statements;
            int statement_count;
        } block;
        struct {
            struct ASTNode** elements;
            int element_count;
        } array;
        struct {
            char** keys;
            struct ASTNode** values;
            int pair_count;
        } dict;
        struct {
            struct ASTNode* array;
            struct ASTNode* index;
        } index;
        struct {
            struct ASTNode* object;
            char* property;
        } dot;
        struct {
            char* name;
            char** fields;
            int field_count;
            struct ASTNode** methods;
            int method_count;
            char* parent_class;  // 부모 클래스 이름
        } class_def;
        struct {
            char* class_name;
            struct ASTNode** args;
            int arg_count;
        } new_expr;
        struct {
            struct ASTNode* object;
            char* method_name;
            struct ASTNode** args;
            int arg_count;
        } method_call;
        struct {
            struct ASTNode* object;
            char* field_name;
            struct ASTNode* value;
        } field_assign;
        struct {
            char* method_name;
            struct ASTNode** args;
            int arg_count;
        } super_call;
        struct {
            struct ASTNode* try_block;
            char* exception_type;  // NULL이면 모든 예외, 아니면 특정 타입만
            char* exception_var;   // catch 블록의 변수명
            struct ASTNode* catch_block;
            struct ASTNode* finally_block;  // NULL일 수 있음
        } try_catch;
        struct {
            struct ASTNode* exception_value;
        } throw_stmt;
        struct {
            struct ASTNode* condition;
            char* message;  // NULL이면 기본 메시지
        } assert_stmt;
        struct {
            char* module_name;  // import할 모듈 이름
            char* alias;        // as로 지정한 별칭 (NULL이면 원래 이름 사용)
        } import_stmt;
        struct {
            struct ASTNode* node;  // export할 노드 (함수, 변수 등)
        } export_stmt;
    } data;
    int line;  // 라인 번호
} ASTNode;

// 파서 구조체
typedef struct {
    Lexer* lexer;
    Token* current_token;
} Parser;

// 함수 선언
Parser* parser_create(Lexer* lexer);
void parser_free(Parser* parser);
void parser_advance(Parser* parser);
ASTNode* parser_parse(Parser* parser);
ASTNode* parser_parse_statement(Parser* parser);
ASTNode* parser_parse_let(Parser* parser);
ASTNode* parser_parse_function(Parser* parser);
ASTNode* parser_parse_if(Parser* parser);
ASTNode* parser_parse_for(Parser* parser);
ASTNode* parser_parse_while(Parser* parser);
ASTNode* parser_parse_return(Parser* parser);
ASTNode* parser_parse_try_catch(Parser* parser);
ASTNode* parser_parse_throw(Parser* parser);
ASTNode* parser_parse_assert(Parser* parser);
ASTNode* parser_parse_import(Parser* parser);
ASTNode* parser_parse_export(Parser* parser);
ASTNode* parser_parse_expression(Parser* parser);
ASTNode* parser_parse_comparison(Parser* parser);
ASTNode* parser_parse_term(Parser* parser);
ASTNode* parser_parse_factor(Parser* parser);
ASTNode* parser_parse_unary(Parser* parser);
ASTNode* parser_parse_postfix(Parser* parser);
ASTNode* parser_parse_primary(Parser* parser);
ASTNode* parser_parse_array(Parser* parser);
ASTNode* parser_parse_dict(Parser* parser);
ASTNode* parser_parse_class(Parser* parser);
ASTNode* parser_parse_block(Parser* parser);
ASTNode* ast_create_number(double value);
ASTNode* ast_create_string(char* value);
ASTNode* ast_create_identifier(char* name);
ASTNode* ast_create_binary(char* op, ASTNode* left, ASTNode* right);
ASTNode* ast_create_function_call(char* name, ASTNode** args, int arg_count);
void ast_free(ASTNode* node);

#endif
