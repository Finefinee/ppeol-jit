#include "module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// 모듈 생성
Module* module_create(const char* name, const char* filepath) {
    Module* module = (Module*)malloc(sizeof(Module));
    module->name = strdup(name);
    module->filepath = strdup(filepath);
    module->ast = NULL;
    module->exports = environment_create(NULL);
    module->is_loaded = 0;
    return module;
}

// 모듈 메모리 해제
void module_free(Module* module) {
    if (!module) return;
    
    free(module->name);
    free(module->filepath);
    if (module->ast) {
        ast_free(module->ast);
    }
    if (module->exports) {
        environment_free(module->exports);
    }
    free(module);
}

// 모듈 경로 해석
// 예: "math" -> "stdlib/math.fine" 또는 "./mymodule" -> "./mymodule.fine"
char* module_resolve_path(const char* module_name, const char* base_path) {
    char* path = (char*)malloc(512);
    
    // 상대 경로인 경우 (. 또는 /로 시작)
    if (module_name[0] == '.' || module_name[0] == '/') {
        snprintf(path, 512, "%s.fine", module_name);
    } else {
        // 표준 라이브러리 경로에서 찾기
        // 먼저 stdlib/ 디렉토리 확인
        snprintf(path, 512, "stdlib/%s.fine", module_name);
        
        // 파일이 없으면 현재 디렉토리에서 찾기
        if (access(path, F_OK) != 0) {
            snprintf(path, 512, "%s.fine", module_name);
        }
    }
    
    return path;
}

// 모듈 로드
Module* module_load(const char* name, const char* base_path) {
    // 경로 해석
    char* filepath = module_resolve_path(name, base_path);
    
    // 파일 열기
    FILE* file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open module file '%s'\n", filepath);
        free(filepath);
        return NULL;
    }
    
    // 파일 크기 확인
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // 파일 내용 읽기
    char* source = (char*)malloc(file_size + 1);
    fread(source, 1, file_size, file);
    source[file_size] = '\0';
    fclose(file);
    
    // 렉싱 및 파싱
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    ASTNode* ast = parser_parse(parser);
    
    // 모듈 생성
    Module* module = module_create(name, filepath);
    module->ast = ast;
    module->is_loaded = 1;
    
    // 정리
    parser_free(parser);
    lexer_free(lexer);
    free(source);
    free(filepath);
    
    return module;
}

// 모듈의 AST 가져오기
ASTNode* module_get_ast(Module* module) {
    return module ? module->ast : NULL;
}

// 모듈의 exports 환경 가져오기
Environment* module_get_exports(Module* module) {
    return module ? module->exports : NULL;
}

// 모듈 캐시 생성
ModuleCache* module_cache_create() {
    ModuleCache* cache = (ModuleCache*)malloc(sizeof(ModuleCache));
    cache->capacity = 10;
    cache->count = 0;
    cache->modules = (Module**)malloc(sizeof(Module*) * cache->capacity);
    return cache;
}

// 모듈 캐시 메모리 해제
void module_cache_free(ModuleCache* cache) {
    if (!cache) return;
    
    for (int i = 0; i < cache->count; i++) {
        module_free(cache->modules[i]);
    }
    free(cache->modules);
    free(cache);
}

// 캐시에서 모듈 찾기
Module* module_cache_get(ModuleCache* cache, const char* name) {
    if (!cache) return NULL;
    
    for (int i = 0; i < cache->count; i++) {
        if (strcmp(cache->modules[i]->name, name) == 0) {
            return cache->modules[i];
        }
    }
    return NULL;
}

// 캐시에 모듈 추가
void module_cache_add(ModuleCache* cache, Module* module) {
    if (!cache || !module) return;
    
    // 용량 확인 및 확장
    if (cache->count >= cache->capacity) {
        cache->capacity *= 2;
        cache->modules = (Module**)realloc(cache->modules, 
                                           sizeof(Module*) * cache->capacity);
    }
    
    cache->modules[cache->count++] = module;
}
