#ifndef MODULE_H
#define MODULE_H

#include "parser.h"
#include "interpreter.h"

// 모듈 구조체
typedef struct Module {
    char* name;                    // 모듈 이름
    char* filepath;                // 모듈 파일 경로
    ASTNode* ast;                  // 파싱된 AST
    Environment* exports;          // export된 심볼들
    int is_loaded;                 // 로드 여부
} Module;

// 모듈 캐시 (이미 로드된 모듈 저장)
typedef struct ModuleCache {
    Module** modules;
    int count;
    int capacity;
} ModuleCache;

// 함수 선언
Module* module_create(const char* name, const char* filepath);
void module_free(Module* module);
Module* module_load(const char* name, const char* base_path);
ASTNode* module_get_ast(Module* module);
Environment* module_get_exports(Module* module);

ModuleCache* module_cache_create();
void module_cache_free(ModuleCache* cache);
Module* module_cache_get(ModuleCache* cache, const char* name);
void module_cache_add(ModuleCache* cache, Module* module);

char* module_resolve_path(const char* module_name, const char* base_path);

#endif
