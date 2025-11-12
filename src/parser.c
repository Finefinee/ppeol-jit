#include "parser.h"
#include <string.h>

// 파서 생성
Parser* parser_create(Lexer* lexer) {
    Parser* parser = (Parser*)malloc(sizeof(Parser));
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    return parser;
}

// 파서 메모리 해제
void parser_free(Parser* parser) {
    token_free(parser->current_token);
    free(parser);
}

// 다음 토큰으로 이동
void parser_advance(Parser* parser) {
    token_free(parser->current_token);
    parser->current_token = lexer_next_token(parser->lexer);
    
    // 줄바꿈 토큰 건너뛰기
    while (parser->current_token->type == TOKEN_NEWLINE) {
        token_free(parser->current_token);
        parser->current_token = lexer_next_token(parser->lexer);
    }
}

// 프로그램 파싱
ASTNode* parser_parse(Parser* parser) {
    ASTNode* program = (ASTNode*)malloc(sizeof(ASTNode));
    program->type = AST_PROGRAM;
    
    int capacity = 10;
    program->data.block.statements = (ASTNode**)malloc(sizeof(ASTNode*) * capacity);
    program->data.block.statement_count = 0;
    
    while (parser->current_token->type != TOKEN_EOF) {
        if (parser->current_token->type == TOKEN_NEWLINE) {
            parser_advance(parser);
            continue;
        }
        
        ASTNode* stmt = parser_parse_statement(parser);
        if (stmt) {
            if (program->data.block.statement_count >= capacity) {
                capacity *= 2;
                program->data.block.statements = (ASTNode**)realloc(
                    program->data.block.statements, sizeof(ASTNode*) * capacity);
            }
            program->data.block.statements[program->data.block.statement_count++] = stmt;
        }
    }
    
    return program;
}

// 문장 파싱
ASTNode* parser_parse_statement(Parser* parser) {
    if (parser->current_token->type == TOKEN_IMPORT || parser->current_token->type == TOKEN_FROM) {
        return parser_parse_import(parser);
    } else if (parser->current_token->type == TOKEN_EXPORT) {
        return parser_parse_export(parser);
    } else if (parser->current_token->type == TOKEN_LET) {
        return parser_parse_let(parser);
    } else if (parser->current_token->type == TOKEN_FN) {
        return parser_parse_function(parser);
    } else if (parser->current_token->type == TOKEN_CLASS) {
        return parser_parse_class(parser);
    } else if (parser->current_token->type == TOKEN_IF) {
        return parser_parse_if(parser);
    } else if (parser->current_token->type == TOKEN_FOR || parser->current_token->type == TOKEN_PARALLEL) {
        return parser_parse_for(parser);
    } else if (parser->current_token->type == TOKEN_WHILE) {
        return parser_parse_while(parser);
    } else if (parser->current_token->type == TOKEN_RETURN) {
        return parser_parse_return(parser);
    } else if (parser->current_token->type == TOKEN_TRY) {
        return parser_parse_try_catch(parser);
    } else if (parser->current_token->type == TOKEN_THROW) {
        return parser_parse_throw(parser);
    } else if (parser->current_token->type == TOKEN_ASSERT) {
        return parser_parse_assert(parser);
    } else {
        return parser_parse_expression(parser);
    }
}

// let 문 파싱
ASTNode* parser_parse_let(Parser* parser) {
    parser_advance(parser); // 'let' 건너뛰기
    
    char* name = strdup(parser->current_token->value);
    parser_advance(parser); // 변수명
    
    parser_advance(parser); // '=' 건너뛰기
    
    ASTNode* value = parser_parse_expression(parser);
    
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_LET;
    node->data.assign.name = name;
    node->data.assign.value = value;
    
    return node;
}

// 함수 정의 파싱
ASTNode* parser_parse_function(Parser* parser) {
    parser_advance(parser); // 'fn' 건너뛰기
    
    char* name = strdup(parser->current_token->value);
    parser_advance(parser);
    
    parser_advance(parser); // '(' 건너뛰기
    
    int param_capacity = 5;
    char** params = (char**)malloc(sizeof(char*) * param_capacity);
    int param_count = 0;
    
    while (parser->current_token->type != TOKEN_RPAREN) {
        if (param_count >= param_capacity) {
            param_capacity *= 2;
            params = (char**)realloc(params, sizeof(char*) * param_capacity);
        }
        params[param_count++] = strdup(parser->current_token->value);
        parser_advance(parser);
        
        if (parser->current_token->type == TOKEN_COMMA) {
            parser_advance(parser);
        }
    }
    
    parser_advance(parser); // ')' 건너뛰기
    
    ASTNode* body = parser_parse_block(parser);
    
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_FUNCTION_DEF;
    node->data.function_def.name = name;
    node->data.function_def.params = params;
    node->data.function_def.param_count = param_count;
    node->data.function_def.body = body;
    
    return node;
}

// if 문 파싱
ASTNode* parser_parse_if(Parser* parser) {
    parser_advance(parser); // 'if' 건너뛰기
    
    ASTNode* condition = parser_parse_expression(parser);
    ASTNode* then_branch = parser_parse_block(parser);
    ASTNode* else_branch = NULL;
    
    if (parser->current_token->type == TOKEN_ELSE) {
        parser_advance(parser);
        else_branch = parser_parse_block(parser);
    }
    
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_IF;
    node->data.if_stmt.condition = condition;
    node->data.if_stmt.then_branch = then_branch;
    node->data.if_stmt.else_branch = else_branch;
    
    return node;
}

// for 루프 파싱
ASTNode* parser_parse_for(Parser* parser) {
    int is_parallel = 0;
    if (parser->current_token->type == TOKEN_PARALLEL) {
        is_parallel = 1;
        parser_advance(parser);
    }
    
    parser_advance(parser); // 'for' 건너뛰기
    
    if (parser->current_token->type == TOKEN_LPAREN) {
        parser_advance(parser); // '(' 건너뛰기
    }
    
    char* iterator = strdup(parser->current_token->value);
    parser_advance(parser);
    
    parser_advance(parser); // 'in' 건너뛰기
    
    ASTNode* iterable = parser_parse_expression(parser);
    
    if (parser->current_token->type == TOKEN_RPAREN) {
        parser_advance(parser); // ')' 건너뛰기
    }
    
    ASTNode* body = parser_parse_block(parser);
    
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_FOR;
    node->data.for_loop.iterator = iterator;
    node->data.for_loop.iterable = iterable;
    node->data.for_loop.body = body;
    node->data.for_loop.is_parallel = is_parallel;
    
    return node;
}

// while 루프 파싱
ASTNode* parser_parse_while(Parser* parser) {
    parser_advance(parser); // 'while' 건너뛰기
    
    ASTNode* condition = parser_parse_expression(parser);
    ASTNode* body = parser_parse_block(parser);
    
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_WHILE;
    node->data.while_loop.condition = condition;
    node->data.while_loop.body = body;
    
    return node;
}

// return 문 파싱
ASTNode* parser_parse_return(Parser* parser) {
    parser_advance(parser); // 'return' 건너뛰기
    
    ASTNode* value = parser_parse_expression(parser);
    
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_RETURN;
    node->data.return_stmt.value = value;
    
    return node;
}

// try-catch 문 파싱
ASTNode* parser_parse_try_catch(Parser* parser) {
    parser_advance(parser); // 'try' 건너뛰기
    
    // try 블록 파싱
    ASTNode* try_block = parser_parse_block(parser);
    
    // catch 블록 파싱
    char* exception_type = NULL;
    char* exception_var = NULL;
    ASTNode* catch_block = NULL;
    
    if (parser->current_token->type == TOKEN_CATCH) {
        parser_advance(parser); // 'catch' 건너뛰기
        
        // 예외 타입 (선택적): catch ZeroDivisionError as e
        if (parser->current_token->type == TOKEN_IDENTIFIER) {
            // 다음 토큰이 'as'인지 확인
            exception_type = strdup(parser->current_token->value);
            parser_advance(parser);
            
            // 'as' 키워드가 있으면 변수명을 파싱
            if (parser->current_token->type == TOKEN_AS) {
                parser_advance(parser); // 'as' 건너뛰기
                
                if (parser->current_token->type == TOKEN_IDENTIFIER) {
                    exception_var = strdup(parser->current_token->value);
                    parser_advance(parser);
                }
            } else {
                // 'as' 없으면 타입이 아니라 변수명이었음
                exception_var = exception_type;
                exception_type = NULL;
            }
        }
        
        // catch 블록
        catch_block = parser_parse_block(parser);
    }
    
    // finally 블록 파싱 (선택적)
    ASTNode* finally_block = NULL;
    if (parser->current_token->type == TOKEN_FINALLY) {
        parser_advance(parser); // 'finally' 건너뛰기
        finally_block = parser_parse_block(parser);
    }
    
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_TRY_CATCH;
    node->data.try_catch.try_block = try_block;
    node->data.try_catch.exception_type = exception_type;
    node->data.try_catch.exception_var = exception_var;
    node->data.try_catch.catch_block = catch_block;
    node->data.try_catch.finally_block = finally_block;
    
    return node;
}

// throw 문 파싱
ASTNode* parser_parse_throw(Parser* parser) {
    parser_advance(parser); // 'throw' 건너뛰기
    
    ASTNode* exception_value = parser_parse_expression(parser);
    
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_THROW;
    node->data.throw_stmt.exception_value = exception_value;
    
    return node;
}

// assert 문 파싱
ASTNode* parser_parse_assert(Parser* parser) {
    parser_advance(parser); // 'assert' 건너뛰기
    
    // 조건 파싱
    ASTNode* condition = parser_parse_expression(parser);
    
    // 선택적 메시지 (콤마 뒤에 문자열)
    char* message = NULL;
    if (parser->current_token->type == TOKEN_COMMA) {
        parser_advance(parser); // ',' 건너뛰기
        
        if (parser->current_token->type == TOKEN_STRING) {
            message = strdup(parser->current_token->value);
            parser_advance(parser);
        }
    }
    
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_ASSERT;
    node->data.assert_stmt.condition = condition;
    node->data.assert_stmt.message = message;
    
    return node;
}

// import 문 파싱
// import 문 파싱
// 문법: import module_name [as alias] 또는 from module_name import name1, name2
ASTNode* parser_parse_import(Parser* parser) {
    // 'from module import ...' 형태 체크
    if (parser->current_token->type == TOKEN_FROM) {
        parser_advance(parser); // 'from' 건너뛰기
        
        if (parser->current_token->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Error: Expected module name after 'from'\n");
            exit(1);
        }
        
        char* module_name = strdup(parser->current_token->value);
        parser_advance(parser);
        
        // 'import' 키워드 확인
        if (parser->current_token->type != TOKEN_IMPORT) {
            fprintf(stderr, "Error: Expected 'import' after module name\n");
            exit(1);
        }
        parser_advance(parser); // 'import' 건너뛰기
        
        // 임포트할 이름들 파싱
        int capacity = 5;
        char** names = (char**)malloc(sizeof(char*) * capacity);
        int count = 0;
        
        while (parser->current_token->type == TOKEN_IDENTIFIER) {
            if (count >= capacity) {
                capacity *= 2;
                names = (char**)realloc(names, sizeof(char*) * capacity);
            }
            names[count++] = strdup(parser->current_token->value);
            parser_advance(parser);
            
            if (parser->current_token->type == TOKEN_COMMA) {
                parser_advance(parser);
            } else {
                break;
            }
        }
        
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_IMPORT;
        node->data.import_stmt.module_name = module_name;
        node->data.import_stmt.alias = NULL;
        node->data.import_stmt.names = names;
        node->data.import_stmt.name_count = count;
        
        return node;
    }
    
    // 'import module [as alias]' 형태
    parser_advance(parser); // 'import' 건너뛰기
    
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected module name after 'import'\n");
        exit(1);
    }
    
    char* module_name = strdup(parser->current_token->value);
    parser_advance(parser);
    
    char* alias = NULL;
    if (parser->current_token->type == TOKEN_AS) {
        parser_advance(parser); // 'as' 건너뛰기
        
        if (parser->current_token->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Error: Expected alias name after 'as'\n");
            exit(1);
        }
        
        alias = strdup(parser->current_token->value);
        parser_advance(parser);
    }
    
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_IMPORT;
    node->data.import_stmt.module_name = module_name;
    node->data.import_stmt.alias = alias;
    node->data.import_stmt.names = NULL;
    node->data.import_stmt.name_count = 0;
    
    return node;
}

// export 문 파싱
// 문법: export fn/let/class ...
ASTNode* parser_parse_export(Parser* parser) {
    parser_advance(parser); // 'export' 건너뛰기
    
    // export 뒤에 오는 것을 파싱 (함수, 변수, 클래스 등)
    ASTNode* exported_node = NULL;
    
    if (parser->current_token->type == TOKEN_FN) {
        exported_node = parser_parse_function(parser);
    } else if (parser->current_token->type == TOKEN_LET) {
        exported_node = parser_parse_let(parser);
    } else if (parser->current_token->type == TOKEN_CLASS) {
        exported_node = parser_parse_class(parser);
    } else {
        fprintf(stderr, "Error: Expected fn, let, or class after 'export'\n");
        exit(1);
    }
    
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_EXPORT;
    node->data.export_stmt.node = exported_node;
    
    return node;
}

// 클래스 정의 파싱
ASTNode* parser_parse_class(Parser* parser) {
    parser_advance(parser); // 'class' 건너뛰기
    
    char* class_name = strdup(parser->current_token->value);
    parser_advance(parser); // 클래스명
    
    // extends 처리
    char* parent_class = NULL;
    if (parser->current_token->type == TOKEN_EXTENDS) {
        parser_advance(parser); // 'extends' 건너뛰기
        parent_class = strdup(parser->current_token->value);
        parser_advance(parser); // 부모 클래스명
    }
    
    parser_advance(parser); // '{' 건너뛰기
    
    ASTNode* class_node = (ASTNode*)malloc(sizeof(ASTNode));
    class_node->type = AST_CLASS;
    class_node->data.class_def.name = class_name;
    class_node->data.class_def.parent_class = parent_class;
    
    int field_capacity = 10;
    class_node->data.class_def.fields = (char**)malloc(sizeof(char*) * field_capacity);
    class_node->data.class_def.field_count = 0;
    
    int method_capacity = 10;
    class_node->data.class_def.methods = (ASTNode**)malloc(sizeof(ASTNode*) * method_capacity);
    class_node->data.class_def.method_count = 0;
    
    while (parser->current_token->type != TOKEN_RBRACE) {
        if (parser->current_token->type == TOKEN_LET) {
            // 필드 정의
            parser_advance(parser); // 'let' 건너뛰기
            char* field_name = strdup(parser->current_token->value);
            
            if (class_node->data.class_def.field_count >= field_capacity) {
                field_capacity *= 2;
                class_node->data.class_def.fields = (char**)realloc(
                    class_node->data.class_def.fields, sizeof(char*) * field_capacity);
            }
            class_node->data.class_def.fields[class_node->data.class_def.field_count++] = field_name;
            parser_advance(parser);
        } else if (parser->current_token->type == TOKEN_FN) {
            // 메서드 정의
            ASTNode* method = parser_parse_function(parser);
            
            if (class_node->data.class_def.method_count >= method_capacity) {
                method_capacity *= 2;
                class_node->data.class_def.methods = (ASTNode**)realloc(
                    class_node->data.class_def.methods, sizeof(ASTNode*) * method_capacity);
            }
            class_node->data.class_def.methods[class_node->data.class_def.method_count++] = method;
        } else {
            parser_advance(parser);
        }
    }
    
    parser_advance(parser); // '}' 건너뛰기
    
    return class_node;
}

// 블록 파싱
ASTNode* parser_parse_block(Parser* parser) {
    parser_advance(parser); // '{' 건너뛰기
    
    ASTNode* block = (ASTNode*)malloc(sizeof(ASTNode));
    block->type = AST_BLOCK;
    
    int capacity = 10;
    block->data.block.statements = (ASTNode**)malloc(sizeof(ASTNode*) * capacity);
    block->data.block.statement_count = 0;
    
    while (parser->current_token->type != TOKEN_RBRACE && parser->current_token->type != TOKEN_EOF) {
        if (parser->current_token->type == TOKEN_NEWLINE) {
            parser_advance(parser);
            continue;
        }
        
        ASTNode* stmt = parser_parse_statement(parser);
        if (stmt) {
            if (block->data.block.statement_count >= capacity) {
                capacity *= 2;
                block->data.block.statements = (ASTNode**)realloc(
                    block->data.block.statements, sizeof(ASTNode*) * capacity);
            }
            block->data.block.statements[block->data.block.statement_count++] = stmt;
        }
    }
    
    parser_advance(parser); // '}' 건너뛰기
    
    return block;
}

// 표현식 파싱 (할당 포함)
ASTNode* parser_parse_expression(Parser* parser) {
    ASTNode* left = parser_parse_comparison(parser);
    
    if (parser->current_token->type == TOKEN_ASSIGN) {
        parser_advance(parser);
        ASTNode* right = parser_parse_expression(parser);
        
        // 필드 할당 처리 (obj.field = value)
        if (left->type == AST_DOT_ACCESS) {
            ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
            node->type = AST_FIELD_ASSIGN;
            node->data.field_assign.object = left->data.dot.object;
            node->data.field_assign.field_name = left->data.dot.property;
            node->data.field_assign.value = right;
            // left의 외부 구조만 해제 (내부는 node가 사용 중)
            free(left);
            return node;
        }
        
        // 일반 변수 할당
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_ASSIGN;
        node->data.assign.name = strdup(left->data.string);
        node->data.assign.value = right;
        ast_free(left);
        return node;
    }
    
    return left;
}

// 비교 연산 파싱
ASTNode* parser_parse_comparison(Parser* parser) {
    ASTNode* left = parser_parse_term(parser);
    
    while (parser->current_token->type == TOKEN_EQUAL ||
           parser->current_token->type == TOKEN_NOT_EQUAL ||
           parser->current_token->type == TOKEN_LESS ||
           parser->current_token->type == TOKEN_LESS_EQUAL ||
           parser->current_token->type == TOKEN_GREATER ||
           parser->current_token->type == TOKEN_GREATER_EQUAL) {
        char* op = strdup(parser->current_token->value);
        parser_advance(parser);
        ASTNode* right = parser_parse_term(parser);
        left = ast_create_binary(op, left, right);
    }
    
    return left;
}

// 덧셈/뺄셈 파싱
ASTNode* parser_parse_term(Parser* parser) {
    ASTNode* left = parser_parse_factor(parser);
    
    while (parser->current_token->type == TOKEN_PLUS ||
           parser->current_token->type == TOKEN_MINUS ||
           parser->current_token->type == TOKEN_AT) {
        char* op = strdup(parser->current_token->value);
        parser_advance(parser);
        ASTNode* right = parser_parse_factor(parser);
        left = ast_create_binary(op, left, right);
    }
    
    return left;
}

// 곱셈/나눗셈 파싱
ASTNode* parser_parse_factor(Parser* parser) {
    ASTNode* left = parser_parse_unary(parser);
    
    while (parser->current_token->type == TOKEN_MULTIPLY ||
           parser->current_token->type == TOKEN_DIVIDE ||
           parser->current_token->type == TOKEN_MODULO ||
           parser->current_token->type == TOKEN_FLOOR_DIV) {
        char* op = strdup(parser->current_token->value);
        parser_advance(parser);
        ASTNode* right = parser_parse_unary(parser);
        left = ast_create_binary(op, left, right);
    }
    
    return left;
}

// 단항 연산자 파싱
ASTNode* parser_parse_unary(Parser* parser) {
    // 단항 마이너스
    if (parser->current_token->type == TOKEN_MINUS) {
        char* op = strdup(parser->current_token->value);
        parser_advance(parser);
        ASTNode* operand = parser_parse_unary(parser);  // 재귀적으로 단항 연산자 처리
        
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_UNARY_OP;
        node->data.unary.op = op;
        node->data.unary.operand = operand;
        return node;
    }
    
    // 단항 연산자가 아니면 postfix로
    return parser_parse_postfix(parser);
}

// 후위 연산 (함수 호출, 배열 인덱싱, 점 접근) 파싱
ASTNode* parser_parse_postfix(Parser* parser) {
    ASTNode* node = parser_parse_primary(parser);
    
    while (1) {
        if (parser->current_token->type == TOKEN_LPAREN) {
            // 함수 호출
            parser_advance(parser);
            
            int arg_capacity = 5;
            ASTNode** args = (ASTNode**)malloc(sizeof(ASTNode*) * arg_capacity);
            int arg_count = 0;
            
            while (parser->current_token->type != TOKEN_RPAREN) {
                if (arg_count >= arg_capacity) {
                    arg_capacity *= 2;
                    args = (ASTNode**)realloc(args, sizeof(ASTNode*) * arg_capacity);
                }
                args[arg_count++] = parser_parse_expression(parser);
                
                if (parser->current_token->type == TOKEN_COMMA) {
                    parser_advance(parser);
                }
            }
            
            parser_advance(parser); // ')' 건너뛰기
            
            char* func_name = strdup(node->data.string);
            int func_line = node->line;  // 함수 이름의 라인 번호
            ast_free(node);
            node = ast_create_function_call(func_name, args, arg_count);
            node->line = func_line;  // 함수 이름의 라인 번호 사용
            
        } else if (parser->current_token->type == TOKEN_LBRACKET) {
            // 배열 인덱싱
            parser_advance(parser);
            ASTNode* index = parser_parse_expression(parser);
            parser_advance(parser); // ']' 건너뛰기
            
            ASTNode* index_node = (ASTNode*)malloc(sizeof(ASTNode));
            index_node->type = AST_INDEX;
            index_node->data.index.array = node;
            index_node->data.index.index = index;
            index_node->line = parser->current_token->line;
            node = index_node;
            
        } else if (parser->current_token->type == TOKEN_DOT) {
            // 점 접근 또는 메서드 호출
            parser_advance(parser);
            char* property = strdup(parser->current_token->value);
            parser_advance(parser);
            
            // 메서드 호출인지 확인
            if (parser->current_token->type == TOKEN_LPAREN) {
                parser_advance(parser); // '(' 건너뛰기
                
                int arg_capacity = 5;
                ASTNode** args = (ASTNode**)malloc(sizeof(ASTNode*) * arg_capacity);
                int arg_count = 0;
                
                while (parser->current_token->type != TOKEN_RPAREN) {
                    if (arg_count >= arg_capacity) {
                        arg_capacity *= 2;
                        args = (ASTNode**)realloc(args, sizeof(ASTNode*) * arg_capacity);
                    }
                    args[arg_count++] = parser_parse_expression(parser);
                    
                    if (parser->current_token->type == TOKEN_COMMA) {
                        parser_advance(parser);
                    }
                }
                
                parser_advance(parser); // ')' 건너뛰기
                
                ASTNode* method_node = (ASTNode*)malloc(sizeof(ASTNode));
                method_node->type = AST_METHOD_CALL;
                method_node->data.method_call.object = node;
                method_node->data.method_call.method_name = property;
                method_node->data.method_call.args = args;
                method_node->data.method_call.arg_count = arg_count;
                node = method_node;
            } else {
                // 필드 접근
                ASTNode* dot_node = (ASTNode*)malloc(sizeof(ASTNode));
                dot_node->type = AST_DOT_ACCESS;
                dot_node->data.dot.object = node;
                dot_node->data.dot.property = property;
                node = dot_node;
            }
            
        } else if (parser->current_token->type == TOKEN_ARROW) {
            // -> 연산자 (메서드 호출)
            parser_advance(parser);
            char* method_name = strdup(parser->current_token->value);
            parser_advance(parser);
            parser_advance(parser); // '(' 건너뛰기
            
            int arg_capacity = 5;
            ASTNode** args = (ASTNode**)malloc(sizeof(ASTNode*) * arg_capacity);
            int arg_count = 0;
            
            while (parser->current_token->type != TOKEN_RPAREN) {
                if (arg_count >= arg_capacity) {
                    arg_capacity *= 2;
                    args = (ASTNode**)realloc(args, sizeof(ASTNode*) * arg_capacity);
                }
                args[arg_count++] = parser_parse_expression(parser);
                
                if (parser->current_token->type == TOKEN_COMMA) {
                    parser_advance(parser);
                }
            }
            
            parser_advance(parser); // ')' 건너뛰기
            
            ASTNode* method_node = (ASTNode*)malloc(sizeof(ASTNode));
            method_node->type = AST_METHOD_CALL;
            method_node->data.method_call.object = node;
            method_node->data.method_call.method_name = method_name;
            method_node->data.method_call.args = args;
            method_node->data.method_call.arg_count = arg_count;
            node = method_node;
            
        } else {
            break;
        }
    }
    
    return node;
}

// 기본 표현식 파싱
ASTNode* parser_parse_primary(Parser* parser) {
    if (parser->current_token->type == TOKEN_NEW) {
        parser_advance(parser); // 'new' 건너뛰기
        
        char* class_name = strdup(parser->current_token->value);
        parser_advance(parser);
        
        // 생성자 인자
        parser_advance(parser); // '(' 건너뛰기
        
        ASTNode* new_node = (ASTNode*)malloc(sizeof(ASTNode));
        new_node->type = AST_NEW;
        new_node->data.new_expr.class_name = class_name;
        
        int capacity = 10;
        new_node->data.new_expr.args = (ASTNode**)malloc(sizeof(ASTNode*) * capacity);
        new_node->data.new_expr.arg_count = 0;
        
        while (parser->current_token->type != TOKEN_RPAREN) {
            if (new_node->data.new_expr.arg_count >= capacity) {
                capacity *= 2;
                new_node->data.new_expr.args = (ASTNode**)realloc(
                    new_node->data.new_expr.args, sizeof(ASTNode*) * capacity);
            }
            new_node->data.new_expr.args[new_node->data.new_expr.arg_count++] = parser_parse_expression(parser);
            
            if (parser->current_token->type == TOKEN_COMMA) {
                parser_advance(parser);
            }
        }
        
        parser_advance(parser); // ')' 건너뛰기
        
        return new_node;
    }
    
    if (parser->current_token->type == TOKEN_THIS) {
        parser_advance(parser);
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_THIS;
        return node;
    }
    
    if (parser->current_token->type == TOKEN_SUPER) {
        parser_advance(parser); // 'super' 건너뛰기
        parser_advance(parser); // '.' 건너뛰기
        
        char* method_name = strdup(parser->current_token->value);
        parser_advance(parser);
        parser_advance(parser); // '(' 건너뛰기
        
        ASTNode* super_node = (ASTNode*)malloc(sizeof(ASTNode));
        super_node->type = AST_SUPER;
        super_node->data.super_call.method_name = method_name;
        
        int capacity = 10;
        super_node->data.super_call.args = (ASTNode**)malloc(sizeof(ASTNode*) * capacity);
        super_node->data.super_call.arg_count = 0;
        
        while (parser->current_token->type != TOKEN_RPAREN) {
            if (super_node->data.super_call.arg_count >= capacity) {
                capacity *= 2;
                super_node->data.super_call.args = (ASTNode**)realloc(
                    super_node->data.super_call.args, sizeof(ASTNode*) * capacity);
            }
            super_node->data.super_call.args[super_node->data.super_call.arg_count++] = parser_parse_expression(parser);
            
            if (parser->current_token->type == TOKEN_COMMA) {
                parser_advance(parser);
            }
        }
        
        parser_advance(parser); // ')' 건너뛰기
        
        return super_node;
    }
    
    if (parser->current_token->type == TOKEN_NUMBER) {
        double value = atof(parser->current_token->value);
        int line = parser->current_token->line;
        parser_advance(parser);
        ASTNode* node = ast_create_number(value);
        node->line = line;
        return node;
    }
    
    if (parser->current_token->type == TOKEN_TRUE) {
        int line = parser->current_token->line;
        parser_advance(parser);
        ASTNode* node = ast_create_bool(1);
        node->line = line;
        return node;
    }
    
    if (parser->current_token->type == TOKEN_FALSE) {
        int line = parser->current_token->line;
        parser_advance(parser);
        ASTNode* node = ast_create_bool(0);
        node->line = line;
        return node;
    }
    
    if (parser->current_token->type == TOKEN_STRING) {
        char* value = strdup(parser->current_token->value);
        int line = parser->current_token->line;
        parser_advance(parser);
        ASTNode* node = ast_create_string(value);
        node->line = line;
        return node;
    }
    
    if (parser->current_token->type == TOKEN_IDENTIFIER ||
        parser->current_token->type == TOKEN_PRINT ||
        parser->current_token->type == TOKEN_RANGE ||
        parser->current_token->type == TOKEN_LEN ||
        parser->current_token->type == TOKEN_SUM) {
        char* name = strdup(parser->current_token->value);
        int line = parser->current_token->line;
        parser_advance(parser);
        ASTNode* node = ast_create_identifier(name);
        node->line = line;
        return node;
    }
    
    if (parser->current_token->type == TOKEN_LBRACKET) {
        return parser_parse_array(parser);
    }
    
    // 중괄호는 무조건 딕셔너리로 처리 (블록은 if/for/while/fn에서만)
    if (parser->current_token->type == TOKEN_LBRACE) {
        return parser_parse_dict(parser);
    }
    
    if (parser->current_token->type == TOKEN_LPAREN) {
        parser_advance(parser);
        ASTNode* expr = parser_parse_expression(parser);
        parser_advance(parser); // ')' 건너뛰기
        return expr;
    }
    
    return NULL;
}

// 배열 파싱
ASTNode* parser_parse_array(Parser* parser) {
    parser_advance(parser); // '[' 건너뛰기
    
    ASTNode* array = (ASTNode*)malloc(sizeof(ASTNode));
    array->type = AST_ARRAY;
    
    int capacity = 10;
    array->data.array.elements = (ASTNode**)malloc(sizeof(ASTNode*) * capacity);
    array->data.array.element_count = 0;
    
    while (parser->current_token->type != TOKEN_RBRACKET) {
        if (array->data.array.element_count >= capacity) {
            capacity *= 2;
            array->data.array.elements = (ASTNode**)realloc(
                array->data.array.elements, sizeof(ASTNode*) * capacity);
        }
        array->data.array.elements[array->data.array.element_count++] = parser_parse_expression(parser);
        
        if (parser->current_token->type == TOKEN_COMMA) {
            parser_advance(parser);
        }
    }
    
    parser_advance(parser); // ']' 건너뛰기
    
    // 중첩 배열인지 확인 (행렬 변환)
    if (array->data.array.element_count > 0) {
        int is_matrix = 1;
        int first_cols = -1;
        
        // 모든 요소가 배열이고 같은 길이인지 확인
        for (int i = 0; i < array->data.array.element_count; i++) {
            if (array->data.array.elements[i]->type != AST_ARRAY) {
                is_matrix = 0;
                break;
            }
            int cols = array->data.array.elements[i]->data.array.element_count;
            if (first_cols == -1) {
                first_cols = cols;
            } else if (cols != first_cols) {
                is_matrix = 0;
                break;
            }
        }
        
        // 행렬로 변환
        if (is_matrix && first_cols > 0) {
            ASTNode* matrix = (ASTNode*)malloc(sizeof(ASTNode));
            matrix->type = AST_MATRIX;
            matrix->data.matrix.row_count = array->data.array.element_count;
            matrix->data.matrix.col_count = first_cols;
            matrix->data.matrix.rows = array->data.array.elements;
            return matrix;
        }
    }
    
    return array;
}

// 딕셔너리 파싱
ASTNode* parser_parse_dict(Parser* parser) {
    parser_advance(parser); // '{' 건너뛰기
    
    ASTNode* dict = (ASTNode*)malloc(sizeof(ASTNode));
    dict->type = AST_DICT;
    
    int capacity = 10;
    dict->data.dict.keys = (char**)malloc(sizeof(char*) * capacity);
    dict->data.dict.values = (ASTNode**)malloc(sizeof(ASTNode*) * capacity);
    dict->data.dict.pair_count = 0;
    
    while (parser->current_token->type != TOKEN_RBRACE) {
        if (dict->data.dict.pair_count >= capacity) {
            capacity *= 2;
            dict->data.dict.keys = (char**)realloc(dict->data.dict.keys, sizeof(char*) * capacity);
            dict->data.dict.values = (ASTNode**)realloc(dict->data.dict.values, sizeof(ASTNode*) * capacity);
        }
        
        // 키 (식별자 또는 문자열)
        char* key;
        if (parser->current_token->type == TOKEN_IDENTIFIER) {
            key = strdup(parser->current_token->value);
        } else if (parser->current_token->type == TOKEN_STRING) {
            key = strdup(parser->current_token->value);
        } else {
            break;
        }
        parser_advance(parser);
        
        // ':' 건너뛰기
        if (parser->current_token->type == TOKEN_COLON) {
            parser_advance(parser);
        }
        
        // 값
        ASTNode* value = parser_parse_expression(parser);
        
        dict->data.dict.keys[dict->data.dict.pair_count] = key;
        dict->data.dict.values[dict->data.dict.pair_count] = value;
        dict->data.dict.pair_count++;
        
        if (parser->current_token->type == TOKEN_COMMA) {
            parser_advance(parser);
        }
    }
    
    parser_advance(parser); // '}' 건너뛰기
    
    return dict;
}

// AST 노드 생성 함수들
ASTNode* ast_create_number(double value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_NUMBER;
    node->data.number = value;
    node->line = 0;  // 호출하는 곳에서 설정
    return node;
}

ASTNode* ast_create_bool(int value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_BOOL;
    node->data.boolean = value ? 1 : 0;
    node->line = 0;  // 호출하는 곳에서 설정
    return node;
}

ASTNode* ast_create_string(char* value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_STRING;
    node->data.string = strdup(value);
    node->line = 0;  // 호출하는 곳에서 설정
    return node;
}

ASTNode* ast_create_identifier(char* name) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_IDENTIFIER;
    node->data.string = strdup(name);
    node->line = 0;  // 호출하는 곳에서 설정
    return node;
}

ASTNode* ast_create_binary(char* op, ASTNode* left, ASTNode* right) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_BINARY_OP;
    node->data.binary.op = strdup(op);
    node->data.binary.left = left;
    node->data.binary.right = right;
    node->line = (left && left->line > 0) ? left->line : 0;  // 왼쪽 피연산자의 라인 사용
    return node;
}

ASTNode* ast_create_function_call(char* name, ASTNode** args, int arg_count) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_FUNCTION_CALL;
    node->data.function_call.name = name;
    node->data.function_call.args = args;
    node->data.function_call.arg_count = arg_count;
    node->line = 0;  // 호출하는 곳에서 설정
    return node;
}

// AST 메모리 해제
void ast_free(ASTNode* node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_STRING:
        case AST_IDENTIFIER:
            free(node->data.string);
            break;
        case AST_BINARY_OP:
            free(node->data.binary.op);
            ast_free(node->data.binary.left);
            ast_free(node->data.binary.right);
            break;
        case AST_ASSIGN:
        case AST_LET:
            free(node->data.assign.name);
            ast_free(node->data.assign.value);
            break;
        case AST_FUNCTION_DEF:
            free(node->data.function_def.name);
            for (int i = 0; i < node->data.function_def.param_count; i++) {
                free(node->data.function_def.params[i]);
            }
            free(node->data.function_def.params);
            ast_free(node->data.function_def.body);
            break;
        case AST_FUNCTION_CALL:
            free(node->data.function_call.name);
            for (int i = 0; i < node->data.function_call.arg_count; i++) {
                ast_free(node->data.function_call.args[i]);
            }
            free(node->data.function_call.args);
            break;
        case AST_IF:
            ast_free(node->data.if_stmt.condition);
            ast_free(node->data.if_stmt.then_branch);
            ast_free(node->data.if_stmt.else_branch);
            break;
        case AST_FOR:
            free(node->data.for_loop.iterator);
            ast_free(node->data.for_loop.iterable);
            ast_free(node->data.for_loop.body);
            break;
        case AST_WHILE:
            ast_free(node->data.while_loop.condition);
            ast_free(node->data.while_loop.body);
            break;
        case AST_RETURN:
            ast_free(node->data.return_stmt.value);
            break;
        case AST_BLOCK:
        case AST_PROGRAM:
            for (int i = 0; i < node->data.block.statement_count; i++) {
                ast_free(node->data.block.statements[i]);
            }
            free(node->data.block.statements);
            break;
        case AST_ARRAY:
            for (int i = 0; i < node->data.array.element_count; i++) {
                ast_free(node->data.array.elements[i]);
            }
            free(node->data.array.elements);
            break;
        case AST_DICT:
            for (int i = 0; i < node->data.dict.pair_count; i++) {
                free(node->data.dict.keys[i]);
                ast_free(node->data.dict.values[i]);
            }
            free(node->data.dict.keys);
            free(node->data.dict.values);
            break;
        case AST_INDEX:
            ast_free(node->data.index.array);
            ast_free(node->data.index.index);
            break;
        case AST_DOT_ACCESS:
            ast_free(node->data.dot.object);
            free(node->data.dot.property);
            break;
        case AST_CLASS:
            free(node->data.class_def.name);
            if (node->data.class_def.parent_class) {
                free(node->data.class_def.parent_class);
            }
            for (int i = 0; i < node->data.class_def.field_count; i++) {
                free(node->data.class_def.fields[i]);
            }
            free(node->data.class_def.fields);
            for (int i = 0; i < node->data.class_def.method_count; i++) {
                ast_free(node->data.class_def.methods[i]);
            }
            free(node->data.class_def.methods);
            break;
        case AST_NEW:
            free(node->data.new_expr.class_name);
            for (int i = 0; i < node->data.new_expr.arg_count; i++) {
                ast_free(node->data.new_expr.args[i]);
            }
            free(node->data.new_expr.args);
            break;
        case AST_METHOD_CALL:
            ast_free(node->data.method_call.object);
            free(node->data.method_call.method_name);
            for (int i = 0; i < node->data.method_call.arg_count; i++) {
                ast_free(node->data.method_call.args[i]);
            }
            free(node->data.method_call.args);
            break;
        case AST_THIS:
            // this는 메모리 해제할 것이 없음
            break;
        case AST_FIELD_ASSIGN:
            ast_free(node->data.field_assign.object);
            free(node->data.field_assign.field_name);
            ast_free(node->data.field_assign.value);
            break;
        case AST_SUPER:
            free(node->data.super_call.method_name);
            for (int i = 0; i < node->data.super_call.arg_count; i++) {
                ast_free(node->data.super_call.args[i]);
            }
            free(node->data.super_call.args);
            break;
        case AST_TRY_CATCH:
            ast_free(node->data.try_catch.try_block);
            if (node->data.try_catch.exception_type) {
                free(node->data.try_catch.exception_type);
            }
            if (node->data.try_catch.exception_var) {
                free(node->data.try_catch.exception_var);
            }
            if (node->data.try_catch.catch_block) {
                ast_free(node->data.try_catch.catch_block);
            }
            if (node->data.try_catch.finally_block) {
                ast_free(node->data.try_catch.finally_block);
            }
            break;
        case AST_THROW:
            ast_free(node->data.throw_stmt.exception_value);
            break;
        case AST_ASSERT:
            ast_free(node->data.assert_stmt.condition);
            if (node->data.assert_stmt.message) {
                free(node->data.assert_stmt.message);
            }
            break;
        default:
            break;
    }
    
    free(node);
}
