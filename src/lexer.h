#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 토큰 타입 정의
typedef enum {
    TOKEN_EOF,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,
    TOKEN_LET,
    TOKEN_FN,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_FOR,
    TOKEN_WHILE,
    TOKEN_RETURN,
    TOKEN_IN,
    TOKEN_PARALLEL,
    TOKEN_PRINT,
    TOKEN_RANGE,
    TOKEN_LEN,
    TOKEN_SUM,
    // map, filter, reduce는 내장 함수 (키워드 아님)
    TOKEN_STRUCT,
    TOKEN_NEW,
    TOKEN_CLASS,
    TOKEN_THIS,
    TOKEN_ARROW,        // ->
    TOKEN_EXTENDS,
    TOKEN_SUPER,
    TOKEN_TRY,
    TOKEN_CATCH,
    TOKEN_THROW,
    TOKEN_FINALLY,
    TOKEN_AS,
    TOKEN_ASSERT,
    TOKEN_IMPORT,       // import
    TOKEN_EXPORT,       // export
    TOKEN_FROM,         // from (for "import x from module")
    TOKEN_TRUE,         // true
    TOKEN_FALSE,        // false
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_MODULO,       // % (나머지)
    TOKEN_FLOOR_DIV,    // // (몫)
    TOKEN_AT,           // @ (내적 연산)
    TOKEN_ASSIGN,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_COLON,
    TOKEN_NEWLINE,
    TOKEN_ERROR
} TokenType;

// 토큰 구조체
typedef struct {
    TokenType type;
    char* value;
    int line;
    int column;
} Token;

// 렉서 구조체
typedef struct {
    char* source;
    int position;
    int line;
    int column;
    char current_char;
} Lexer;

// 함수 선언
Lexer* lexer_create(char* source);
void lexer_free(Lexer* lexer);
void lexer_advance(Lexer* lexer);
void lexer_skip_whitespace(Lexer* lexer);
void lexer_skip_comment(Lexer* lexer);
Token* lexer_next_token(Lexer* lexer);
Token* lexer_read_number(Lexer* lexer);
Token* lexer_read_string(Lexer* lexer);
Token* lexer_read_identifier(Lexer* lexer);
Token* token_create(TokenType type, char* value, int line, int column);
void token_free(Token* token);
const char* token_type_to_string(TokenType type);

#endif
