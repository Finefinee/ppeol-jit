#include "lexer.h"

// 렉서 생성
Lexer* lexer_create(char* source) {
    Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
    lexer->source = source;
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->current_char = source[0];
    return lexer;
}

// 렉서 메모리 해제
void lexer_free(Lexer* lexer) {
    free(lexer);
}

// 다음 문자로 이동
void lexer_advance(Lexer* lexer) {
    if (lexer->current_char == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    
    lexer->position++;
    if (lexer->position < strlen(lexer->source)) {
        lexer->current_char = lexer->source[lexer->position];
    } else {
        lexer->current_char = '\0';
    }
}

// 공백 건너뛰기
void lexer_skip_whitespace(Lexer* lexer) {
    while (lexer->current_char != '\0' && isspace(lexer->current_char) && lexer->current_char != '\n') {
        lexer_advance(lexer);
    }
}

// 주석 건너뛰기
void lexer_skip_comment(Lexer* lexer) {
    if (lexer->current_char == '#') {
        while (lexer->current_char != '\n' && lexer->current_char != '\0') {
            lexer_advance(lexer);
        }
    }
}

// 숫자 읽기
Token* lexer_read_number(Lexer* lexer) {
    int start_column = lexer->column;
    int start_pos = lexer->position;
    int has_dot = 0;
    
    while (isdigit(lexer->current_char) || lexer->current_char == '.') {
        if (lexer->current_char == '.') {
            if (has_dot) break;
            has_dot = 1;
        }
        lexer_advance(lexer);
    }
    
    int length = lexer->position - start_pos;
    char* value = (char*)malloc(length + 1);
    strncpy(value, &lexer->source[start_pos], length);
    value[length] = '\0';
    
    Token* token = token_create(TOKEN_NUMBER, value, lexer->line, start_column);
    free(value);
    return token;
}

// 문자열 읽기
Token* lexer_read_string(Lexer* lexer) {
    int start_column = lexer->column;
    lexer_advance(lexer); // 여는 따옴표 건너뛰기
    
    // 동적 버퍼로 문자열 구성 (이스케이프 처리 위해)
    int capacity = 64;
    int length = 0;
    char* value = (char*)malloc(capacity);
    
    while (lexer->current_char != '"' && lexer->current_char != '\0') {
        // 버퍼 확장 필요시
        if (length >= capacity - 1) {
            capacity *= 2;
            value = (char*)realloc(value, capacity);
        }
        
        // 이스케이프 시퀀스 처리
        if (lexer->current_char == '\\') {
            lexer_advance(lexer);
            switch (lexer->current_char) {
                case 'n':  value[length++] = '\n'; break;  // 개행
                case 't':  value[length++] = '\t'; break;  // 탭
                case 'r':  value[length++] = '\r'; break;  // 캐리지 리턴
                case '\\': value[length++] = '\\'; break;  // 백슬래시
                case '"':  value[length++] = '"';  break;  // 큰따옴표
                case '\'': value[length++] = '\''; break;  // 작은따옴표
                case '0':  value[length++] = '\0'; break;  // 널 문자
                default:
                    // 알 수 없는 이스케이프 시퀀스는 그대로 추가
                    value[length++] = '\\';
                    value[length++] = lexer->current_char;
                    break;
            }
            lexer_advance(lexer);
        } else {
            value[length++] = lexer->current_char;
            lexer_advance(lexer);
        }
    }
    
    value[length] = '\0';
    
    if (lexer->current_char == '"') {
        lexer_advance(lexer); // 닫는 따옴표 건너뛰기
    }
    
    Token* token = token_create(TOKEN_STRING, value, lexer->line, start_column);
    free(value);
    return token;
}

// 식별자 또는 키워드 읽기
Token* lexer_read_identifier(Lexer* lexer) {
    int start_column = lexer->column;
    int start_pos = lexer->position;
    
    while (isalnum(lexer->current_char) || lexer->current_char == '_') {
        lexer_advance(lexer);
    }
    
    int length = lexer->position - start_pos;
    char* value = (char*)malloc(length + 1);
    strncpy(value, &lexer->source[start_pos], length);
    value[length] = '\0';
    
    // 키워드 확인
    TokenType type = TOKEN_IDENTIFIER;
    if (strcmp(value, "let") == 0) type = TOKEN_LET;
    else if (strcmp(value, "fn") == 0) type = TOKEN_FN;
    else if (strcmp(value, "if") == 0) type = TOKEN_IF;
    else if (strcmp(value, "else") == 0) type = TOKEN_ELSE;
    else if (strcmp(value, "for") == 0) type = TOKEN_FOR;
    else if (strcmp(value, "while") == 0) type = TOKEN_WHILE;
    else if (strcmp(value, "return") == 0) type = TOKEN_RETURN;
    else if (strcmp(value, "in") == 0) type = TOKEN_IN;
    else if (strcmp(value, "parallel") == 0) type = TOKEN_PARALLEL;
    else if (strcmp(value, "print") == 0) type = TOKEN_PRINT;
    else if (strcmp(value, "range") == 0) type = TOKEN_RANGE;
    else if (strcmp(value, "len") == 0) type = TOKEN_LEN;
    else if (strcmp(value, "sum") == 0) type = TOKEN_SUM;
    // map, filter, reduce는 내장 함수로 처리 (키워드 아님)
    else if (strcmp(value, "struct") == 0) type = TOKEN_STRUCT;
    else if (strcmp(value, "new") == 0) type = TOKEN_NEW;
    else if (strcmp(value, "class") == 0) type = TOKEN_CLASS;
    else if (strcmp(value, "this") == 0) type = TOKEN_THIS;
    else if (strcmp(value, "extends") == 0) type = TOKEN_EXTENDS;
    else if (strcmp(value, "super") == 0) type = TOKEN_SUPER;
    else if (strcmp(value, "try") == 0) type = TOKEN_TRY;
    else if (strcmp(value, "catch") == 0) type = TOKEN_CATCH;
    else if (strcmp(value, "throw") == 0) type = TOKEN_THROW;
    else if (strcmp(value, "finally") == 0) type = TOKEN_FINALLY;
    else if (strcmp(value, "as") == 0) type = TOKEN_AS;
    else if (strcmp(value, "assert") == 0) type = TOKEN_ASSERT;
    else if (strcmp(value, "import") == 0) type = TOKEN_IMPORT;
    else if (strcmp(value, "export") == 0) type = TOKEN_EXPORT;
    else if (strcmp(value, "from") == 0) type = TOKEN_FROM;
    else if (strcmp(value, "true") == 0) type = TOKEN_TRUE;
    else if (strcmp(value, "false") == 0) type = TOKEN_FALSE;
    
    Token* token = token_create(type, value, lexer->line, start_column);
    free(value);
    return token;
}

// 다음 토큰 가져오기
Token* lexer_next_token(Lexer* lexer) {
    while (lexer->current_char != '\0') {
        if (isspace(lexer->current_char) && lexer->current_char != '\n') {
            lexer_skip_whitespace(lexer);
            continue;
        }
        
        if (lexer->current_char == '#') {
            lexer_skip_comment(lexer);
            continue;
        }
        
        if (lexer->current_char == '\n') {
            Token* token = token_create(TOKEN_NEWLINE, "\n", lexer->line, lexer->column);
            lexer_advance(lexer);
            return token;
        }
        
        if (isdigit(lexer->current_char)) {
            return lexer_read_number(lexer);
        }
        
        if (lexer->current_char == '"') {
            return lexer_read_string(lexer);
        }
        
        if (isalpha(lexer->current_char) || lexer->current_char == '_') {
            return lexer_read_identifier(lexer);
        }
        
        int column = lexer->column;
        char ch = lexer->current_char;
        lexer_advance(lexer);
        
        switch (ch) {
            case '+': return token_create(TOKEN_PLUS, "+", lexer->line, column);
            case '-':
                if (lexer->current_char == '>') {
                    lexer_advance(lexer);
                    return token_create(TOKEN_ARROW, "->", lexer->line, column);
                }
                return token_create(TOKEN_MINUS, "-", lexer->line, column);
            case '*': return token_create(TOKEN_MULTIPLY, "*", lexer->line, column);
            case '/':
                if (lexer->current_char == '/') {
                    // 몫 연산자 //
                    lexer_advance(lexer);  // 두 번째 / 건너뛰기
                    return token_create(TOKEN_FLOOR_DIV, "//", lexer->line, column);
                }
                return token_create(TOKEN_DIVIDE, "/", lexer->line, column);
            case '%': return token_create(TOKEN_MODULO, "%", lexer->line, column);
            case '@': return token_create(TOKEN_AT, "@", lexer->line, column);
            case '(': return token_create(TOKEN_LPAREN, "(", lexer->line, column);
            case ')': return token_create(TOKEN_RPAREN, ")", lexer->line, column);
            case '{': return token_create(TOKEN_LBRACE, "{", lexer->line, column);
            case '}': return token_create(TOKEN_RBRACE, "}", lexer->line, column);
            case '[': return token_create(TOKEN_LBRACKET, "[", lexer->line, column);
            case ']': return token_create(TOKEN_RBRACKET, "]", lexer->line, column);
            case ',': return token_create(TOKEN_COMMA, ",", lexer->line, column);
            case '.': return token_create(TOKEN_DOT, ".", lexer->line, column);
            case ':': return token_create(TOKEN_COLON, ":", lexer->line, column);
            case '=':
                if (lexer->current_char == '=') {
                    lexer_advance(lexer);
                    return token_create(TOKEN_EQUAL, "==", lexer->line, column);
                }
                return token_create(TOKEN_ASSIGN, "=", lexer->line, column);
            case '!':
                if (lexer->current_char == '=') {
                    lexer_advance(lexer);
                    return token_create(TOKEN_NOT_EQUAL, "!=", lexer->line, column);
                }
                break;
            case '<':
                if (lexer->current_char == '=') {
                    lexer_advance(lexer);
                    return token_create(TOKEN_LESS_EQUAL, "<=", lexer->line, column);
                }
                return token_create(TOKEN_LESS, "<", lexer->line, column);
            case '>':
                if (lexer->current_char == '=') {
                    lexer_advance(lexer);
                    return token_create(TOKEN_GREATER_EQUAL, ">=", lexer->line, column);
                }
                return token_create(TOKEN_GREATER, ">", lexer->line, column);
        }
        
        return token_create(TOKEN_ERROR, "Unknown character", lexer->line, column);
    }
    
    return token_create(TOKEN_EOF, "", lexer->line, lexer->column);
}

// 토큰 생성
Token* token_create(TokenType type, char* value, int line, int column) {
    Token* token = (Token*)malloc(sizeof(Token));
    token->type = type;
    token->value = strdup(value);
    token->line = line;
    token->column = column;
    return token;
}

// 토큰 메모리 해제
void token_free(Token* token) {
    if (token) {
        free(token->value);
        free(token);
    }
}

// 토큰 타입을 문자열로 변환
const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_STRING: return "STRING";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_LET: return "LET";
        case TOKEN_FN: return "FN";
        case TOKEN_IF: return "IF";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_FOR: return "FOR";
        case TOKEN_WHILE: return "WHILE";
        case TOKEN_RETURN: return "RETURN";
        case TOKEN_IN: return "IN";
        case TOKEN_PARALLEL: return "PARALLEL";
        case TOKEN_PRINT: return "PRINT";
        case TOKEN_RANGE: return "RANGE";
        case TOKEN_LEN: return "LEN";
        case TOKEN_SUM: return "SUM";
        case TOKEN_STRUCT: return "STRUCT";
        case TOKEN_NEW: return "NEW";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_MULTIPLY: return "MULTIPLY";
        case TOKEN_DIVIDE: return "DIVIDE";
        case TOKEN_MODULO: return "MODULO";
        case TOKEN_FLOOR_DIV: return "FLOOR_DIV";
        case TOKEN_AT: return "AT";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_EQUAL: return "EQUAL";
        case TOKEN_NOT_EQUAL: return "NOT_EQUAL";
        case TOKEN_LESS: return "LESS";
        case TOKEN_LESS_EQUAL: return "LESS_EQUAL";
        case TOKEN_GREATER: return "GREATER";
        case TOKEN_GREATER_EQUAL: return "GREATER_EQUAL";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_LBRACE: return "LBRACE";
        case TOKEN_RBRACE: return "RBRACE";
        case TOKEN_LBRACKET: return "LBRACKET";
        case TOKEN_RBRACKET: return "RBRACKET";
        case TOKEN_COMMA: return "COMMA";
        case TOKEN_DOT: return "DOT";
        case TOKEN_COLON: return "COLON";
        case TOKEN_CLASS: return "CLASS";
        case TOKEN_THIS: return "THIS";
        case TOKEN_ARROW: return "ARROW";
        case TOKEN_EXTENDS: return "EXTENDS";
        case TOKEN_SUPER: return "SUPER";
        case TOKEN_TRY: return "TRY";
        case TOKEN_CATCH: return "CATCH";
        case TOKEN_THROW: return "THROW";
        case TOKEN_FINALLY: return "FINALLY";
        case TOKEN_AS: return "AS";
        case TOKEN_ASSERT: return "ASSERT";
        case TOKEN_IMPORT: return "IMPORT";
        case TOKEN_EXPORT: return "EXPORT";
        case TOKEN_FROM: return "FROM";
        case TOKEN_TRUE: return "TRUE";
        case TOKEN_FALSE: return "FALSE";
        case TOKEN_NEWLINE: return "NEWLINE";
        case TOKEN_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}
