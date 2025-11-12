#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "compiler.h"
#include "vm.h"
#include "bytecode.h"

// 파일 읽기
char* read_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s'\n", filename);
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* buffer = (char*)malloc(length + 1);
    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    
    fclose(file);
    return buffer;
}

// REPL (Read-Eval-Print Loop)
void repl() {
    printf("FineLang v1.0 - Interactive Mode\n");
    printf("Type 'exit' to quit\n\n");
    
    Interpreter* interp = interpreter_create();
    char line[1024];
    
    while (1) {
        printf(">>> ");
        if (!fgets(line, sizeof(line), stdin)) break;
        
        // 줄바꿈 제거
        line[strcspn(line, "\n")] = 0;
        
        if (strcmp(line, "exit") == 0) break;
        if (strlen(line) == 0) continue;
        
        Lexer* lexer = lexer_create(line);
        Parser* parser = parser_create(lexer);
        ASTNode* ast = parser_parse(parser);
        
        Value* result = interpreter_eval(interp, ast);
        
        // 예외 처리
        if (interp->has_exception && interp->current_exception) {
            value_print(interp->current_exception);
            printf("\n");
            interp->has_exception = 0;
            value_free(interp->current_exception);
            interp->current_exception = NULL;
        } else if (result && result->type != VAL_NULL) {
            value_print(result);
            printf("\n");
        }
        
        value_free(result);
        ast_free(ast);
        parser_free(parser);
        lexer_free(lexer);
    }
    
    interpreter_free(interp);
}

// 파일 실행 (인터프리터 모드)
void run_file(const char* filename) {
    char* source = read_file(filename);
    if (!source) {
        exit(1);
    }
    
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    ASTNode* ast = parser_parse(parser);
    
    Interpreter* interp = interpreter_create();
    interpreter_eval(interp, ast);
    
    // 예외 처리
    if (interp->has_exception && interp->current_exception) {
        value_print(interp->current_exception);
        printf("\n");
    }
    
    // 프로그램 정상 종료 - OS가 메모리 정리
    exit(0);
}

// 파일 실행 (VM 모드)
void run_file_vm(const char* filename) {
    char* source = read_file(filename);
    if (!source) {
        exit(1);
    }
    
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    ASTNode* ast = parser_parse(parser);
    
    // AST를 bytecode로 컴파일
    BytecodeChunk* chunk = compile(ast);
    
    printf("\n=== Bytecode Disassembly ===\n");
    bytecode_disassemble(chunk, filename);
    printf("\n=== Execution ===\n");
    
    // VM 실행
    VM* vm = vm_create();
    vm_run(vm, chunk);
    
    vm_free(vm);
    bytecode_chunk_free(chunk);
    ast_free(ast);
    parser_free(parser);
    lexer_free(lexer);
    free(source);
    
    exit(0);
}

// 사용법 출력
void print_usage(const char* program) {
    printf("FineLang - AI-friendly programming language\n\n");
    printf("Usage:\n");
    printf("  %s                  Start interactive mode (REPL)\n", program);
    printf("  %s <file.fine>     Run a FineLang program (interpreter mode)\n", program);
    printf("  %s --vm <file.fine> Run a FineLang program (VM mode)\n", program);
    printf("  %s -v <file.fine>   Run a FineLang program (VM mode, short)\n", program);
    printf("  %s -h, --help      Show this help message\n", program);
    printf("\nExamples:\n");
    printf("  %s                 # Start REPL\n", program);
    printf("  %s hello.fine      # Run hello.fine (interpreter)\n", program);
    printf("  %s --vm test.fine  # Run test.fine (bytecode VM)\n", program);
    printf("  %s -v test.fine    # Run test.fine (bytecode VM)\n", program);
}

int main(int argc, char** argv) {
    if (argc == 1) {
        // REPL 모드
        repl();
    } else if (argc == 2) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_usage(argv[0]);
        } else {
            // 파일 실행 (인터프리터 모드)
            run_file(argv[1]);
        }
    } else if (argc == 3) {
        if (strcmp(argv[1], "--vm") == 0 || strcmp(argv[1], "-v") == 0) {
            // 파일 실행 (VM 모드)
            run_file_vm(argv[2]);
        } else {
            print_usage(argv[0]);
            return 1;
        }
    } else {
        print_usage(argv[0]);
        return 1;
    }
    
    return 0;
}
