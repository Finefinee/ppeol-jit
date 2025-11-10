#include "interpreter.h"
#include "module.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

// 인터프리터 생성
Interpreter* interpreter_create() {
    Interpreter* interp = (Interpreter*)malloc(sizeof(Interpreter));
    interp->global_env = environment_create(NULL);
    interp->current_env = interp->global_env;
    interp->return_value = NULL;
    interp->has_returned = 0;
    interp->current_exception = NULL;
    interp->has_exception = 0;
    interp->call_stack = NULL;
    interp->stack_depth = 0;
    interp->max_stack_depth = 1000;
    interp->current_file = strdup("<input>");
    interp->module_cache = module_cache_create();
    return interp;
}

// 인터프리터 메모리 해제
void interpreter_free(Interpreter* interp) {
    environment_free(interp->global_env);
    if (interp->return_value) {
        value_free(interp->return_value);
    }
    if (interp->module_cache) {
        module_cache_free(interp->module_cache);
    }
    free(interp);
}

// AST 노드 평가
Value* interpreter_eval(Interpreter* interp, ASTNode* node) {
    if (!node) return value_create_null();
    
    // 예외 전파
    if (interp->has_exception) {
        return value_create_null();
    }
    
    if (interp->has_returned) {
        return value_create_null();
    }
    
    switch (node->type) {
        case AST_NUMBER:
            return value_create_number(node->data.number);
            
        case AST_STRING:
            return value_create_string(node->data.string);
            
        case AST_IDENTIFIER: {
            Value* val = environment_get(interp->current_env, node->data.string);
            return val ? value_copy(val) : value_create_null();
        }
            
        case AST_BINARY_OP:
            return interpreter_eval_binary(interp, node);
            
        case AST_UNARY_OP: {
            Value* operand = interpreter_eval(interp, node->data.unary.operand);
            Value* result = NULL;
            
            if (strcmp(node->data.unary.op, "-") == 0) {
                if (operand->type == VAL_NUMBER) {
                    result = value_create_number(-operand->data.number);
                } else {
                    result = value_create_null();
                }
            } else if (strcmp(node->data.unary.op, "!") == 0) {
                if (operand->type == VAL_NUMBER) {
                    result = value_create_number(operand->data.number == 0 ? 1 : 0);
                } else {
                    result = value_create_null();
                }
            } else {
                result = value_create_null();
            }
            
            value_free(operand);
            return result;
        }
            
        case AST_LET: {
            Value* val = interpreter_eval(interp, node->data.assign.value);
            environment_set(interp->current_env, node->data.assign.name, val);
            return value_create_null();
        }
            
        case AST_ASSIGN: {
            Value* val = interpreter_eval(interp, node->data.assign.value);
            environment_set(interp->current_env, node->data.assign.name, val);
            return value_copy(val);
        }
            
        case AST_FUNCTION_DEF: {
            Value* func = (Value*)malloc(sizeof(Value));
            func->type = VAL_FUNCTION;
            func->data.function.param_count = node->data.function_def.param_count;
            func->data.function.params = malloc(sizeof(char*) * node->data.function_def.param_count);
            for (int i = 0; i < node->data.function_def.param_count; i++) {
                func->data.function.params[i] = strdup(node->data.function_def.params[i]);
            }
            func->data.function.body = node->data.function_def.body;
            func->data.function.closure = interp->current_env;
            environment_set(interp->current_env, node->data.function_def.name, func);
            return value_create_null();
        }
            
        case AST_FUNCTION_CALL:
            return interpreter_eval_function_call(interp, node);
            
        case AST_IF: {
            Value* cond = interpreter_eval(interp, node->data.if_stmt.condition);
            int is_true = (cond->type == VAL_NUMBER && cond->data.number != 0);
            value_free(cond);
            
            if (is_true) {
                Value* result = interpreter_eval(interp, node->data.if_stmt.then_branch);
                return result;
            } else if (node->data.if_stmt.else_branch) {
                Value* result = interpreter_eval(interp, node->data.if_stmt.else_branch);
                return result;
            }
            return value_create_null();
        }
            
        case AST_FOR: {
            Value* iterable = interpreter_eval(interp, node->data.for_loop.iterable);
            
            if (iterable->type == VAL_ARRAY) {
                for (int i = 0; i < iterable->data.array.count; i++) {
                    environment_set(interp->current_env, node->data.for_loop.iterator, 
                                  iterable->data.array.elements[i]);
                    Value* result = interpreter_eval(interp, node->data.for_loop.body);
                    value_free(result);
                    
                    if (interp->has_returned) break;
                }
            }
            
            value_free(iterable);
            return value_create_null();
        }
            
        case AST_WHILE: {
            while (1) {
                Value* cond = interpreter_eval(interp, node->data.while_loop.condition);
                int is_true = (cond->type == VAL_NUMBER && cond->data.number != 0);
                value_free(cond);
                
                if (!is_true) break;
                
                Value* result = interpreter_eval(interp, node->data.while_loop.body);
                value_free(result);
                
                if (interp->has_returned) break;
            }
            return value_create_null();
        }
            
        case AST_RETURN: {
            interp->return_value = interpreter_eval(interp, node->data.return_stmt.value);
            interp->has_returned = 1;
            return value_copy(interp->return_value);
        }
            
        case AST_BLOCK: {
            Value* last = value_create_null();
            for (int i = 0; i < node->data.block.statement_count; i++) {
                value_free(last);
                last = interpreter_eval(interp, node->data.block.statements[i]);
                
                if (interp->has_returned) break;
            }
            return last;
        }
            
        case AST_PROGRAM: {
            Value* last = value_create_null();
            for (int i = 0; i < node->data.block.statement_count; i++) {
                value_free(last);
                last = interpreter_eval(interp, node->data.block.statements[i]);
            }
            return last;
        }
            
        case AST_ARRAY: {
            Value** elements = (Value**)malloc(sizeof(Value*) * node->data.array.element_count);
            for (int i = 0; i < node->data.array.element_count; i++) {
                elements[i] = interpreter_eval(interp, node->data.array.elements[i]);
            }
            return value_create_array(elements, node->data.array.element_count);
        }
        
        case AST_MATRIX: {
            // 행렬 생성
            Value* matrix = value_create_matrix(node->data.matrix.row_count, node->data.matrix.col_count);
            
            // 각 행의 데이터를 행렬에 채우기
            for (int i = 0; i < node->data.matrix.row_count; i++) {
                ASTNode* row_node = node->data.matrix.rows[i];
                for (int j = 0; j < node->data.matrix.col_count; j++) {
                    Value* elem = interpreter_eval(interp, row_node->data.array.elements[j]);
                    if (elem->type == VAL_NUMBER) {
                        matrix->data.matrix.data[i][j] = elem->data.number;
                    } else {
                        matrix->data.matrix.data[i][j] = 0.0;
                    }
                    value_free(elem);
                }
            }
            
            return matrix;
        }
            
        case AST_DICT: {
            char** keys = (char**)malloc(sizeof(char*) * node->data.dict.pair_count);
            Value** values = (Value**)malloc(sizeof(Value*) * node->data.dict.pair_count);
            for (int i = 0; i < node->data.dict.pair_count; i++) {
                keys[i] = strdup(node->data.dict.keys[i]);
                values[i] = interpreter_eval(interp, node->data.dict.values[i]);
            }
            return value_create_dict(keys, values, node->data.dict.pair_count);
        }
            
        case AST_INDEX: {
            Value* array = interpreter_eval(interp, node->data.index.array);
            Value* index = interpreter_eval(interp, node->data.index.index);
            
            if (array->type == VAL_ARRAY && index->type == VAL_NUMBER) {
                int idx = (int)index->data.number;
                if (idx < 0 || idx >= array->data.array.count) {
                    char msg[100];
                    snprintf(msg, sizeof(msg), "list index out of range: %d", idx);
                    interp->current_exception = value_create_exception("IndexError", msg);
                    exception_attach_stack_trace(interp, interp->current_exception);
                    interp->has_exception = 1;
                    value_free(array);
                    value_free(index);
                    return value_create_null();
                }
                Value* result = value_copy(array->data.array.elements[idx]);
                value_free(array);
                value_free(index);
                return result;
            } else if (array->type == VAL_DICT && index->type == VAL_STRING) {
                // 딕셔너리 키로 접근
                for (int i = 0; i < array->data.dict.count; i++) {
                    if (strcmp(array->data.dict.keys[i], index->data.string) == 0) {
                        Value* result = value_copy(array->data.dict.values[i]);
                        value_free(array);
                        value_free(index);
                        return result;
                    }
                }
                // KeyError 발생
                char msg[200];
                snprintf(msg, sizeof(msg), "key not found: '%s'", index->data.string);
                interp->current_exception = value_create_exception("KeyError", msg);
                exception_attach_stack_trace(interp, interp->current_exception);
                interp->has_exception = 1;
                value_free(array);
                value_free(index);
                return value_create_null();
            } else if (array->type == VAL_MATRIX && index->type == VAL_NUMBER) {
                // 행렬 행 접근 (1차원 인덱스는 행을 반환)
                int idx = (int)index->data.number;
                if (idx < 0 || idx >= array->data.matrix.rows) {
                    char msg[100];
                    snprintf(msg, sizeof(msg), "matrix row index out of range: %d", idx);
                    interp->current_exception = value_create_exception("IndexError", msg);
                    exception_attach_stack_trace(interp, interp->current_exception);
                    interp->has_exception = 1;
                    value_free(array);
                    value_free(index);
                    return value_create_null();
                }
                // 행을 배열로 반환
                Value** row_elements = (Value**)malloc(sizeof(Value*) * array->data.matrix.cols);
                for (int j = 0; j < array->data.matrix.cols; j++) {
                    row_elements[j] = value_create_number(array->data.matrix.data[idx][j]);
                }
                Value* result = value_create_array(row_elements, array->data.matrix.cols);
                value_free(array);
                value_free(index);
                return result;
            }
            
            value_free(array);
            value_free(index);
            return value_create_null();
        }
            
        case AST_DOT_ACCESS: {
            Value* obj = interpreter_eval(interp, node->data.dot.object);
            
            // 모듈 exports 접근
            if (obj->type == VAL_MODULE) {
                Value* exported_value = environment_get(obj->data.module.exports, node->data.dot.property);
                if (exported_value) {
                    Value* result = value_copy(exported_value);
                    return result;
                } else {
                    fprintf(stderr, "Error: Module '%s' has no export '%s'\n", 
                            obj->data.module.name, node->data.dot.property);
                    return value_create_null();
                }
            }
            
            // 인스턴스 필드 접근
            if (obj->type == VAL_INSTANCE) {
                for (int i = 0; i < obj->data.instance.field_count; i++) {
                    if (strcmp(obj->data.instance.field_names[i], node->data.dot.property) == 0) {
                        Value* result = value_copy(obj->data.instance.field_values[i]);
                        value_free(obj);
                        return result;
                    }
                }
            }
            
            // 배열 메서드
            if (obj->type == VAL_ARRAY && strcmp(node->data.dot.property, "map") == 0) {
                // map은 함수 호출로 처리됨
                value_free(obj);
                return value_create_null();
            }
            
            value_free(obj);
            return value_create_null();
        }
            
        case AST_CLASS: {
            // 클래스 정의를 환경에 저장
            Value* class_val = (Value*)malloc(sizeof(Value));
            class_val->type = VAL_CLASS;
            class_val->data.class_def.name = strdup(node->data.class_def.name);
            class_val->data.class_def.parent_class = node->data.class_def.parent_class ? 
                strdup(node->data.class_def.parent_class) : NULL;
            
            // 부모 클래스의 필드와 메서드 상속
            int total_fields = node->data.class_def.field_count;
            int total_methods = node->data.class_def.method_count;
            
            Value* parent_val = NULL;
            if (class_val->data.class_def.parent_class) {
                parent_val = environment_get(interp->current_env, class_val->data.class_def.parent_class);
                if (parent_val && parent_val->type == VAL_CLASS) {
                    total_fields += parent_val->data.class_def.field_count;
                    total_methods += parent_val->data.class_def.method_count;
                }
            }
            
            // 필드 병합 (부모 + 자식)
            class_val->data.class_def.field_count = total_fields;
            class_val->data.class_def.fields = (char**)malloc(sizeof(char*) * total_fields);
            int field_idx = 0;
            
            // 부모 필드 복사
            if (parent_val && parent_val->type == VAL_CLASS) {
                for (int i = 0; i < parent_val->data.class_def.field_count; i++) {
                    class_val->data.class_def.fields[field_idx++] = strdup(parent_val->data.class_def.fields[i]);
                }
            }
            
            // 자식 필드 추가
            for (int i = 0; i < node->data.class_def.field_count; i++) {
                class_val->data.class_def.fields[field_idx++] = strdup(node->data.class_def.fields[i]);
            }
            
            // 메서드 병합 (부모 + 자식, 오버라이딩 지원)
            class_val->data.class_def.method_count = total_methods;
            class_val->data.class_def.methods = (ASTNode**)malloc(sizeof(ASTNode*) * total_methods);
            int method_idx = 0;
            
            // 부모 메서드 복사
            if (parent_val && parent_val->type == VAL_CLASS) {
                for (int i = 0; i < parent_val->data.class_def.method_count; i++) {
                    class_val->data.class_def.methods[method_idx++] = parent_val->data.class_def.methods[i];
                }
            }
            
            // 자식 메서드 추가 (오버라이딩 처리)
            for (int i = 0; i < node->data.class_def.method_count; i++) {
                ASTNode* child_method = node->data.class_def.methods[i];
                int overridden = 0;
                
                // 부모 메서드 중 같은 이름이 있으면 오버라이드
                if (parent_val && parent_val->type == VAL_CLASS) {
                    for (int j = 0; j < parent_val->data.class_def.method_count; j++) {
                        if (strcmp(parent_val->data.class_def.methods[j]->data.function_def.name,
                                 child_method->data.function_def.name) == 0) {
                            // 오버라이드: 부모 메서드를 자식 메서드로 교체
                            class_val->data.class_def.methods[j] = child_method;
                            overridden = 1;
                            break;
                        }
                    }
                }
                
                // 오버라이드하지 않았으면 새 메서드 추가
                if (!overridden) {
                    class_val->data.class_def.methods[method_idx++] = child_method;
                }
            }
            
            // 실제 메서드 개수 업데이트
            class_val->data.class_def.method_count = method_idx;
            
            environment_set(interp->current_env, node->data.class_def.name, class_val);
            return value_create_null();
        }
            
        case AST_NEW: {
            // 클래스 인스턴스 생성
            Value* class_val = environment_get(interp->current_env, node->data.new_expr.class_name);
            if (!class_val || class_val->type != VAL_CLASS) {
                return value_create_null();
            }
            
            Value* instance = (Value*)malloc(sizeof(Value));
            instance->type = VAL_INSTANCE;
            instance->data.instance.class_name = strdup(class_val->data.class_def.name);
            instance->data.instance.parent_class = class_val->data.class_def.parent_class ? 
                strdup(class_val->data.class_def.parent_class) : NULL;
            instance->data.instance.field_count = class_val->data.class_def.field_count;
            instance->data.instance.field_names = (char**)malloc(sizeof(char*) * instance->data.instance.field_count);
            instance->data.instance.field_values = (Value**)malloc(sizeof(Value*) * instance->data.instance.field_count);
            
            // 필드 초기화
            for (int i = 0; i < instance->data.instance.field_count; i++) {
                instance->data.instance.field_names[i] = strdup(class_val->data.class_def.fields[i]);
                // 기본값으로 초기화
                instance->data.instance.field_values[i] = value_create_null();
            }
            
            // constructor 메서드 찾아서 실행
            for (int i = 0; i < class_val->data.class_def.method_count; i++) {
                ASTNode* method = class_val->data.class_def.methods[i];
                if (strcmp(method->data.function_def.name, "constructor") == 0) {
                    // 생성자 실행
                    Environment* prev_env = interp->current_env;
                    interp->current_env = environment_create(interp->current_env);
                    
                    // this 바인딩
                    environment_set(interp->current_env, "this", instance);
                    
                    // 생성자 파라미터 바인딩
                    for (int j = 0; j < method->data.function_def.param_count && j < node->data.new_expr.arg_count; j++) {
                        Value* arg = interpreter_eval(interp, node->data.new_expr.args[j]);
                        environment_set(interp->current_env, method->data.function_def.params[j], arg);
                    }
                    
                    // 생성자 바디 실행
                    Value* result = interpreter_eval(interp, method->data.function_def.body);
                    value_free(result);
                    
                    environment_free(interp->current_env);
                    interp->current_env = prev_env;
                    break;
                }
            }
            
            return instance;
        }
            
        case AST_THIS: {
            // this는 현재 인스턴스를 참조
            Value* this_val = environment_get(interp->current_env, "this");
            return this_val ? value_copy(this_val) : value_create_null();
        }
            
        case AST_METHOD_CALL: {
            Value* obj = interpreter_eval(interp, node->data.method_call.object);
            
            // 배열 메서드 호출
            if (obj->type == VAL_ARRAY) {
                const char* method = node->data.method_call.method_name;
                
                // append(value) - 배열 끝에 요소 추가
                if (strcmp(method, "append") == 0 && node->data.method_call.arg_count > 0) {
                    Value* val = interpreter_eval(interp, node->data.method_call.args[0]);
                    
                    // 새 배열 생성 (원본 크기 + 1)
                    Value** new_elements = (Value**)malloc(sizeof(Value*) * (obj->data.array.count + 1));
                    
                    // 기존 요소 복사
                    for (int i = 0; i < obj->data.array.count; i++) {
                        new_elements[i] = value_copy(obj->data.array.elements[i]);
                    }
                    
                    // 새 요소 추가
                    new_elements[obj->data.array.count] = value_copy(val);
                    
                    Value* result = value_create_array(new_elements, obj->data.array.count + 1);
                    value_free(obj);
                    value_free(val);
                    return result;
                }
                
                // reverse() - 배열 뒤집기
                if (strcmp(method, "reverse") == 0) {
                    Value** reversed = (Value**)malloc(sizeof(Value*) * obj->data.array.count);
                    for (int i = 0; i < obj->data.array.count; i++) {
                        reversed[i] = value_copy(obj->data.array.elements[obj->data.array.count - 1 - i]);
                    }
                    Value* result = value_create_array(reversed, obj->data.array.count);
                    value_free(obj);
                    return result;
                }
                
                // contains(value) - 값 포함 여부
                if (strcmp(method, "contains") == 0 && node->data.method_call.arg_count > 0) {
                    Value* search = interpreter_eval(interp, node->data.method_call.args[0]);
                    int found = 0;
                    
                    for (int i = 0; i < obj->data.array.count; i++) {
                        Value* elem = obj->data.array.elements[i];
                        if (elem->type == search->type) {
                            if (elem->type == VAL_NUMBER && elem->data.number == search->data.number) {
                                found = 1;
                                break;
                            } else if (elem->type == VAL_STRING && strcmp(elem->data.string, search->data.string) == 0) {
                                found = 1;
                                break;
                            }
                        }
                    }
                    
                    value_free(obj);
                    value_free(search);
                    return value_create_number(found);
                }
                
                // index_of(value) - 값의 인덱스 찾기
                if (strcmp(method, "index_of") == 0 && node->data.method_call.arg_count > 0) {
                    Value* search = interpreter_eval(interp, node->data.method_call.args[0]);
                    int index = -1;
                    
                    for (int i = 0; i < obj->data.array.count; i++) {
                        Value* elem = obj->data.array.elements[i];
                        if (elem->type == search->type) {
                            if (elem->type == VAL_NUMBER && elem->data.number == search->data.number) {
                                index = i;
                                break;
                            } else if (elem->type == VAL_STRING && strcmp(elem->data.string, search->data.string) == 0) {
                                index = i;
                                break;
                            }
                        }
                    }
                    
                    value_free(obj);
                    value_free(search);
                    return value_create_number(index);
                }
                
                // min() - 최솟값
                if (strcmp(method, "min") == 0 && obj->data.array.count > 0) {
                    double min_val = obj->data.array.elements[0]->data.number;
                    for (int i = 1; i < obj->data.array.count; i++) {
                        if (obj->data.array.elements[i]->type == VAL_NUMBER) {
                            double val = obj->data.array.elements[i]->data.number;
                            if (val < min_val) {
                                min_val = val;
                            }
                        }
                    }
                    value_free(obj);
                    return value_create_number(min_val);
                }
                
                // max() - 최댓값
                if (strcmp(method, "max") == 0 && obj->data.array.count > 0) {
                    double max_val = obj->data.array.elements[0]->data.number;
                    for (int i = 1; i < obj->data.array.count; i++) {
                        if (obj->data.array.elements[i]->type == VAL_NUMBER) {
                            double val = obj->data.array.elements[i]->data.number;
                            if (val > max_val) {
                                max_val = val;
                            }
                        }
                    }
                    value_free(obj);
                    return value_create_number(max_val);
                }
            }
            
            // 문자열 메서드 호출
            if (obj->type == VAL_STRING) {
                const char* method = node->data.method_call.method_name;
                
                // contains(substring) - 부분 문자열 포함 여부
                if (strcmp(method, "contains") == 0 && node->data.method_call.arg_count > 0) {
                    Value* search = interpreter_eval(interp, node->data.method_call.args[0]);
                    int found = 0;
                    
                    if (search->type == VAL_STRING) {
                        found = (strstr(obj->data.string, search->data.string) != NULL) ? 1 : 0;
                    }
                    
                    value_free(obj);
                    value_free(search);
                    return value_create_number(found);
                }
                
                // index_of(substring) - 부분 문자열 인덱스
                if (strcmp(method, "index_of") == 0 && node->data.method_call.arg_count > 0) {
                    Value* search = interpreter_eval(interp, node->data.method_call.args[0]);
                    int index = -1;
                    
                    if (search->type == VAL_STRING) {
                        char* pos = strstr(obj->data.string, search->data.string);
                        if (pos != NULL) {
                            index = pos - obj->data.string;
                        }
                    }
                    
                    value_free(obj);
                    value_free(search);
                    return value_create_number(index);
                }
            }
            
            // 모듈 함수 호출
            if (obj->type == VAL_MODULE) {
                // 모듈의 exports에서 함수 찾기
                Value* func = environment_get(obj->data.module.exports, node->data.method_call.method_name);
                
                if (func && func->type == VAL_FUNCTION) {
                    // 함수 호출 환경 생성
                    Environment* prev_env = interp->current_env;
                    interp->current_env = environment_create(func->data.function.closure);
                    
                    // 인자 평가 및 바인딩
                    for (int i = 0; i < func->data.function.param_count && i < node->data.method_call.arg_count; i++) {
                        Value* arg = interpreter_eval(interp, node->data.method_call.args[i]);
                        environment_set(interp->current_env, func->data.function.params[i], arg);
                    }
                    
                    // 함수 본문 실행
                    int prev_return = interp->has_returned;
                    interp->has_returned = 0;
                    
                    Value* result = interpreter_eval(interp, func->data.function.body);
                    
                    if (interp->has_returned) {
                        value_free(result);
                        result = value_copy(interp->return_value);
                        value_free(interp->return_value);
                        interp->return_value = NULL;
                    }
                    
                    interp->has_returned = prev_return;
                    
                    environment_free(interp->current_env);
                    interp->current_env = prev_env;
                    
                    return result;
                } else {
                    fprintf(stderr, "Error: Module '%s' has no function '%s'\n", 
                            obj->data.module.name, node->data.method_call.method_name);
                    return value_create_null();
                }
            }
            
            // 인스턴스 메서드 호출
            if (obj->type == VAL_INSTANCE) {
                // 클래스에서 메서드 찾기
                Value* class_val = environment_get(interp->current_env, obj->data.instance.class_name);
                if (class_val && class_val->type == VAL_CLASS) {
                    // 메서드 찾기
                    for (int i = 0; i < class_val->data.class_def.method_count; i++) {
                        ASTNode* method = class_val->data.class_def.methods[i];
                        if (strcmp(method->data.function_def.name, node->data.method_call.method_name) == 0) {
                            // 메서드 실행
                            Environment* prev_env = interp->current_env;
                            interp->current_env = environment_create(interp->current_env);
                            
                            // this 바인딩 (현재 인스턴스)
                            environment_set(interp->current_env, "this", obj);
                            
                            // 파라미터 바인딩
                            for (int j = 0; j < method->data.function_def.param_count && j < node->data.method_call.arg_count; j++) {
                                Value* arg = interpreter_eval(interp, node->data.method_call.args[j]);
                                environment_set(interp->current_env, method->data.function_def.params[j], arg);
                            }
                            
                            // 메서드 바디 실행
                            Value* result = interpreter_eval(interp, method->data.function_def.body);
                            
                            environment_free(interp->current_env);
                            interp->current_env = prev_env;
                            
                            value_free(obj);
                            return result;
                        }
                    }
                }
            }
            
            value_free(obj);
            return value_create_null();
        }
            
        case AST_FIELD_ASSIGN: {
            Value* obj = interpreter_eval(interp, node->data.field_assign.object);
            Value* val = interpreter_eval(interp, node->data.field_assign.value);
            
            if (obj->type == VAL_INSTANCE) {
                // 필드 찾아서 수정
                for (int i = 0; i < obj->data.instance.field_count; i++) {
                    if (strcmp(obj->data.instance.field_names[i], node->data.field_assign.field_name) == 0) {
                        value_free(obj->data.instance.field_values[i]);
                        obj->data.instance.field_values[i] = value_copy(val);
                        value_free(obj);
                        return val;
                    }
                }
            }
            
            value_free(obj);
            return val;
        }
            
        case AST_SUPER: {
            // super 메서드 호출
            Value* this_val = environment_get(interp->current_env, "this");
            if (!this_val || this_val->type != VAL_INSTANCE || !this_val->data.instance.parent_class) {
                return value_create_null();
            }
            
            // 부모 클래스 찾기
            Value* parent_class = environment_get(interp->global_env, this_val->data.instance.parent_class);
            if (!parent_class || parent_class->type != VAL_CLASS) {
                return value_create_null();
            }
            
            // 부모 클래스에서 메서드 찾기
            for (int i = 0; i < parent_class->data.class_def.method_count; i++) {
                ASTNode* method = parent_class->data.class_def.methods[i];
                if (strcmp(method->data.function_def.name, node->data.super_call.method_name) == 0) {
                    // 메서드 실행
                    Environment* prev_env = interp->current_env;
                    interp->current_env = environment_create(interp->current_env);
                    
                    // this 바인딩 (현재 인스턴스 유지)
                    environment_set(interp->current_env, "this", this_val);
                    
                    // 파라미터 바인딩
                    for (int j = 0; j < method->data.function_def.param_count && j < node->data.super_call.arg_count; j++) {
                        Value* arg = interpreter_eval(interp, node->data.super_call.args[j]);
                        environment_set(interp->current_env, method->data.function_def.params[j], arg);
                    }
                    
                    // 메서드 바디 실행
                    Value* result = interpreter_eval(interp, method->data.function_def.body);
                    
                    environment_free(interp->current_env);
                    interp->current_env = prev_env;
                    
                    return result;
                }
            }
            
            return value_create_null();
        }
        
        case AST_TRY_CATCH: {
            // try 블록 실행
            Value* try_result = interpreter_eval(interp, node->data.try_catch.try_block);
            
            // 예외 발생 여부 확인
            if (interp->has_exception && node->data.try_catch.catch_block) {
                // 타입별 catch: 예외 타입이 지정되어 있으면 확인
                int type_matches = 1;  // 기본적으로 매치
                if (node->data.try_catch.exception_type) {
                    // 예외 타입 확인
                    if (interp->current_exception->type == VAL_EXCEPTION) {
                        char* actual_type = interp->current_exception->data.exception.type;
                        char* expected_type = node->data.try_catch.exception_type;
                        type_matches = (strcmp(actual_type, expected_type) == 0);
                    } else {
                        type_matches = 0;
                    }
                }
                
                if (type_matches) {
                    // catch 블록 실행
                    Environment* prev_env = interp->current_env;
                    interp->current_env = environment_create(interp->current_env);
                    
                    // 예외 변수 바인딩
                    if (node->data.try_catch.exception_var) {
                        environment_set(interp->current_env, node->data.try_catch.exception_var, 
                                      interp->current_exception);
                    }
                    
                    // 예외 클리어
                    Value* exception = interp->current_exception;
                    interp->current_exception = NULL;
                    interp->has_exception = 0;
                    
                    // catch 블록 실행
                    Value* catch_result = interpreter_eval(interp, node->data.try_catch.catch_block);
                    
                    environment_free(interp->current_env);
                    interp->current_env = prev_env;
                    
                    value_free(try_result);
                    value_free(exception);
                    
                    // finally 블록 실행
                    if (node->data.try_catch.finally_block) {
                        Value* finally_result = interpreter_eval(interp, node->data.try_catch.finally_block);
                        value_free(finally_result);
                    }
                    
                    return catch_result;
                }
                // 타입이 매치하지 않으면 예외는 계속 전파됨
            }
            
            // finally 블록 실행 (예외 없어도)
            if (node->data.try_catch.finally_block) {
                Value* finally_result = interpreter_eval(interp, node->data.try_catch.finally_block);
                value_free(finally_result);
            }
            
            return try_result;
        }
        
        case AST_THROW: {
            // throw 문 처리
            Value* exception_value = interpreter_eval(interp, node->data.throw_stmt.exception_value);
            
            // 예외가 이미 예외 타입이면 그대로 사용
            if (exception_value->type == VAL_EXCEPTION) {
                interp->current_exception = exception_value;
            } else {
                // 문자열이면 RuntimeError로 래핑
                char* message = "";
                if (exception_value->type == VAL_STRING) {
                    message = exception_value->data.string;
                }
                interp->current_exception = value_create_exception("RuntimeError", message);
                value_free(exception_value);
            }
            
            exception_attach_stack_trace(interp, interp->current_exception);
            interp->has_exception = 1;
            return value_create_null();
        }
        
        case AST_ASSERT: {
            // 조건 평가
            Value* condition = interpreter_eval(interp, node->data.assert_stmt.condition);
            
            // 조건이 false이면 AssertionError 발생
            int is_true = 0;
            if (condition->type == VAL_NUMBER) {
                is_true = (condition->data.number != 0);
            }
            
            if (!is_true) {
                // 메시지 생성
                char* message = "Assertion failed";
                if (node->data.assert_stmt.message) {
                    message = node->data.assert_stmt.message;
                }
                
                interp->current_exception = value_create_exception("AssertionError", message);
                exception_attach_stack_trace(interp, interp->current_exception);
                interp->has_exception = 1;
            }
            
            value_free(condition);
            return value_create_null();
        }
        
        case AST_IMPORT: {
            char* module_name = node->data.import_stmt.module_name;
            char* alias = node->data.import_stmt.alias;
            char** names = node->data.import_stmt.names;
            int name_count = node->data.import_stmt.name_count;
            
            // 캐시에서 모듈 확인
            Module* module = module_cache_get(interp->module_cache, module_name);
            
            // 모듈이 캐시에 없으면 로드
            if (!module) {
                module = module_load(module_name, ".");
                if (!module) {
                    fprintf(stderr, "Error: Failed to load module '%s'\n", module_name);
                    return value_create_null();
                }
                
                // 모듈 실행하여 exports 채우기
                Environment* prev_env = interp->current_env;
                interp->current_env = module->exports;
                
                interpreter_eval(interp, module->ast);
                
                interp->current_env = prev_env;
                
                // 캐시에 추가
                module_cache_add(interp->module_cache, module);
            }
            
            // from module import name1, name2 형태인 경우
            if (names != NULL && name_count > 0) {
                // 지정된 이름들만 현재 환경에 추가
                for (int i = 0; i < name_count; i++) {
                    Value* exported = environment_get(module->exports, names[i]);
                    if (exported == NULL) {
                        fprintf(stderr, "Error: Module '%s' does not export '%s'\n", 
                                module_name, names[i]);
                        continue;
                    }
                    environment_set(interp->current_env, names[i], value_copy(exported));
                }
            } else {
                // 일반 import: 모듈 객체 생성
                Value* module_obj = value_create_module(module_name, module->exports);
                
                // 변수 이름 결정 (alias가 있으면 alias 사용, 없으면 module_name 사용)
                char* var_name = alias ? alias : module_name;
                
                // 현재 환경에 모듈 객체를 변수로 저장
                environment_set(interp->current_env, var_name, module_obj);
            }
            
            return value_create_null();
        }
        
        case AST_EXPORT: {
            // export된 노드 평가
            ASTNode* exported = node->data.export_stmt.node;
            
            // 함수나 변수를 현재 환경(모듈의 exports)에 추가
            interpreter_eval(interp, exported);
            
            // export는 정의를 실행하고 null 반환
            return value_create_null();
        }
            
        default:
            return value_create_null();
    }
}

// 이항 연산 평가
Value* interpreter_eval_binary(Interpreter* interp, ASTNode* node) {
    Value* left = interpreter_eval(interp, node->data.binary.left);
    Value* right = interpreter_eval(interp, node->data.binary.right);
    
    char* op = node->data.binary.op;
    Value* result = value_create_null();
    
    if (left->type == VAL_NUMBER && right->type == VAL_NUMBER) {
        double l = left->data.number;
        double r = right->data.number;
        
        if (strcmp(op, "+") == 0) result = value_create_number(l + r);
        else if (strcmp(op, "-") == 0) result = value_create_number(l - r);
        else if (strcmp(op, "*") == 0) result = value_create_number(l * r);
        else if (strcmp(op, "/") == 0) {
            if (r == 0) {
                interp->current_exception = value_create_exception("ZeroDivisionError", "division by zero");
                exception_attach_stack_trace(interp, interp->current_exception);
                interp->has_exception = 1;
                return value_create_null();
            }
            result = value_create_number(l / r);
        }
        else if (strcmp(op, "%") == 0) {
            if (r == 0) {
                interp->current_exception = value_create_exception("ZeroDivisionError", "modulo by zero");
                exception_attach_stack_trace(interp, interp->current_exception);
                interp->has_exception = 1;
                return value_create_null();
            }
            result = value_create_number(fmod(l, r));
        }
        else if (strcmp(op, "//") == 0) {
            if (r == 0) {
                interp->current_exception = value_create_exception("ZeroDivisionError", "floor division by zero");
                exception_attach_stack_trace(interp, interp->current_exception);
                interp->has_exception = 1;
                return value_create_null();
            }
            result = value_create_number(floor(l / r));
        }
        else if (strcmp(op, "==") == 0) result = value_create_number(l == r ? 1 : 0);
        else if (strcmp(op, "!=") == 0) result = value_create_number(l != r ? 1 : 0);
        else if (strcmp(op, "<") == 0) result = value_create_number(l < r ? 1 : 0);
        else if (strcmp(op, "<=") == 0) result = value_create_number(l <= r ? 1 : 0);
        else if (strcmp(op, ">") == 0) result = value_create_number(l > r ? 1 : 0);
        else if (strcmp(op, ">=") == 0) result = value_create_number(l >= r ? 1 : 0);
    } else if (left->type == VAL_STRING && right->type == VAL_STRING) {
        // 문자열 + 문자열 = 연결
        if (strcmp(op, "+") == 0) {
            int len = strlen(left->data.string) + strlen(right->data.string);
            char* concatenated = (char*)malloc(len + 1);
            strcpy(concatenated, left->data.string);
            strcat(concatenated, right->data.string);
            result = value_create_string(concatenated);
            free(concatenated);
        }
        // 문자열 비교
        else if (strcmp(op, "==") == 0) {
            result = value_create_number(strcmp(left->data.string, right->data.string) == 0 ? 1 : 0);
        }
        else if (strcmp(op, "!=") == 0) {
            result = value_create_number(strcmp(left->data.string, right->data.string) != 0 ? 1 : 0);
        }
    } else if (left->type == VAL_STRING && right->type == VAL_NUMBER) {
        // 문자열 * 숫자 = 반복
        if (strcmp(op, "*") == 0) {
            int repeat = (int)right->data.number;
            if (repeat < 0) repeat = 0;
            
            int len = strlen(left->data.string) * repeat;
            char* repeated = (char*)malloc(len + 1);
            repeated[0] = '\0';
            
            for (int i = 0; i < repeat; i++) {
                strcat(repeated, left->data.string);
            }
            
            result = value_create_string(repeated);
            free(repeated);
        }
    } else if (left->type == VAL_NUMBER && right->type == VAL_STRING) {
        // 숫자 * 문자열 = 반복
        if (strcmp(op, "*") == 0) {
            int repeat = (int)left->data.number;
            if (repeat < 0) repeat = 0;
            
            int len = strlen(right->data.string) * repeat;
            char* repeated = (char*)malloc(len + 1);
            repeated[0] = '\0';
            
            for (int i = 0; i < repeat; i++) {
                strcat(repeated, right->data.string);
            }
            
            result = value_create_string(repeated);
            free(repeated);
        }
    } else if (left->type == VAL_ARRAY && right->type == VAL_ARRAY) {
        // 벡터 연산
        if (left->data.array.count == right->data.array.count) {
            int count = left->data.array.count;
            Value** elements = (Value**)malloc(sizeof(Value*) * count);
            
            if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || strcmp(op, "*") == 0) {
                for (int i = 0; i < count; i++) {
                    double l = left->data.array.elements[i]->data.number;
                    double r = right->data.array.elements[i]->data.number;
                    
                    if (strcmp(op, "+") == 0) elements[i] = value_create_number(l + r);
                    else if (strcmp(op, "-") == 0) elements[i] = value_create_number(l - r);
                    else if (strcmp(op, "*") == 0) elements[i] = value_create_number(l * r);
                }
                result = value_create_array(elements, count);
            } else if (strcmp(op, "@") == 0) {
                // 내적
                double sum = 0;
                for (int i = 0; i < count; i++) {
                    double l = left->data.array.elements[i]->data.number;
                    double r = right->data.array.elements[i]->data.number;
                    sum += l * r;
                }
                result = value_create_number(sum);
            }
        }
    } else if (left->type == VAL_MATRIX && right->type == VAL_MATRIX) {
        // 행렬 연산
        if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) {
            // 행렬 덧셈/뺄셈: 크기가 같아야 함
            if (left->data.matrix.rows == right->data.matrix.rows &&
                left->data.matrix.cols == right->data.matrix.cols) {
                
                int rows = left->data.matrix.rows;
                int cols = left->data.matrix.cols;
                Value* result_matrix = value_create_matrix(rows, cols);
                
                for (int i = 0; i < rows; i++) {
                    for (int j = 0; j < cols; j++) {
                        double l = left->data.matrix.data[i][j];
                        double r = right->data.matrix.data[i][j];
                        
                        if (strcmp(op, "+") == 0) {
                            result_matrix->data.matrix.data[i][j] = l + r;
                        } else {
                            result_matrix->data.matrix.data[i][j] = l - r;
                        }
                    }
                }
                result = result_matrix;
            } else {
                // 크기 불일치 에러
                char msg[200];
                snprintf(msg, sizeof(msg), 
                    "matrix dimension mismatch: (%dx%d) %s (%dx%d)",
                    left->data.matrix.rows, left->data.matrix.cols, op,
                    right->data.matrix.rows, right->data.matrix.cols);
                interp->current_exception = value_create_exception("ValueError", msg);
                exception_attach_stack_trace(interp, interp->current_exception);
                interp->has_exception = 1;
                value_free(left);
                value_free(right);
                return value_create_null();
            }
        } else if (strcmp(op, "@") == 0) {
            // 행렬 곱셈: A의 열 수 = B의 행 수
            if (left->data.matrix.cols == right->data.matrix.rows) {
                int m = left->data.matrix.rows;
                int n = right->data.matrix.cols;
                int k = left->data.matrix.cols;
                
                Value* result_matrix = value_create_matrix(m, n);
                
                for (int i = 0; i < m; i++) {
                    for (int j = 0; j < n; j++) {
                        double sum = 0.0;
                        for (int p = 0; p < k; p++) {
                            sum += left->data.matrix.data[i][p] * right->data.matrix.data[p][j];
                        }
                        result_matrix->data.matrix.data[i][j] = sum;
                    }
                }
                result = result_matrix;
            } else {
                // 크기 불일치 에러
                char msg[200];
                snprintf(msg, sizeof(msg), 
                    "matrix dimension mismatch for multiplication: (%dx%d) @ (%dx%d)",
                    left->data.matrix.rows, left->data.matrix.cols,
                    right->data.matrix.rows, right->data.matrix.cols);
                interp->current_exception = value_create_exception("ValueError", msg);
                exception_attach_stack_trace(interp, interp->current_exception);
                interp->has_exception = 1;
                value_free(left);
                value_free(right);
                return value_create_null();
            }
        }
    } else if ((left->type == VAL_MATRIX && right->type == VAL_NUMBER) ||
               (left->type == VAL_NUMBER && right->type == VAL_MATRIX)) {
        // 스칼라 곱: 행렬 * 숫자 또는 숫자 * 행렬
        if (strcmp(op, "*") == 0) {
            Value* matrix = (left->type == VAL_MATRIX) ? left : right;
            double scalar = (left->type == VAL_NUMBER) ? left->data.number : right->data.number;
            
            int rows = matrix->data.matrix.rows;
            int cols = matrix->data.matrix.cols;
            Value* result_matrix = value_create_matrix(rows, cols);
            
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    result_matrix->data.matrix.data[i][j] = matrix->data.matrix.data[i][j] * scalar;
                }
            }
            result = result_matrix;
        }
    }
    
    value_free(left);
    value_free(right);
    return result;
}

// 함수 호출 평가
Value* interpreter_eval_function_call(Interpreter* interp, ASTNode* node) {
    char* name = node->data.function_call.name;
    
    // 내장 함수
    if (strcmp(name, "print") == 0) {
        for (int i = 0; i < node->data.function_call.arg_count; i++) {
            Value* arg = interpreter_eval(interp, node->data.function_call.args[i]);
            value_print(arg);
            if (i < node->data.function_call.arg_count - 1) printf(" ");
            value_free(arg);
        }
        printf("\n");
        return value_create_null();
    }
    
    if (strcmp(name, "range") == 0) {
        if (node->data.function_call.arg_count >= 2) {
            Value* start = interpreter_eval(interp, node->data.function_call.args[0]);
            Value* end = interpreter_eval(interp, node->data.function_call.args[1]);
            
            int s = (int)start->data.number;
            int e = (int)end->data.number;
            int count = e - s;
            
            Value** elements = (Value**)malloc(sizeof(Value*) * count);
            for (int i = 0; i < count; i++) {
                elements[i] = value_create_number(s + i);
            }
            
            value_free(start);
            value_free(end);
            return value_create_array(elements, count);
        }
    }
    
    if (strcmp(name, "len") == 0) {
        if (node->data.function_call.arg_count > 0) {
            Value* arg = interpreter_eval(interp, node->data.function_call.args[0]);
            if (arg->type == VAL_ARRAY) {
                Value* result = value_create_number(arg->data.array.count);
                value_free(arg);
                return result;
            } else if (arg->type == VAL_DICT) {
                Value* result = value_create_number(arg->data.dict.count);
                value_free(arg);
                return result;
            }
            value_free(arg);
        }
    }
    
    if (strcmp(name, "sum") == 0) {
        if (node->data.function_call.arg_count > 0) {
            Value* arg = interpreter_eval(interp, node->data.function_call.args[0]);
            if (arg->type == VAL_ARRAY) {
                double sum = 0;
                for (int i = 0; i < arg->data.array.count; i++) {
                    sum += arg->data.array.elements[i]->data.number;
                }
                value_free(arg);
                return value_create_number(sum);
            }
            value_free(arg);
        }
    }
    
    if (strcmp(name, "keys") == 0) {
        if (node->data.function_call.arg_count > 0) {
            Value* arg = interpreter_eval(interp, node->data.function_call.args[0]);
            if (arg->type == VAL_DICT) {
                Value** elements = (Value**)malloc(sizeof(Value*) * arg->data.dict.count);
                for (int i = 0; i < arg->data.dict.count; i++) {
                    elements[i] = value_create_string(arg->data.dict.keys[i]);
                }
                Value* result = value_create_array(elements, arg->data.dict.count);
                value_free(arg);
                return result;
            }
            value_free(arg);
        }
    }
    
    if (strcmp(name, "values") == 0) {
        if (node->data.function_call.arg_count > 0) {
            Value* arg = interpreter_eval(interp, node->data.function_call.args[0]);
            if (arg->type == VAL_DICT) {
                Value** elements = (Value**)malloc(sizeof(Value*) * arg->data.dict.count);
                for (int i = 0; i < arg->data.dict.count; i++) {
                    elements[i] = value_copy(arg->data.dict.values[i]);
                }
                Value* result = value_create_array(elements, arg->data.dict.count);
                value_free(arg);
                return result;
            }
            value_free(arg);
        }
    }
    
    // 타입 체크 함수들 (v2.2.5)
    if (strcmp(name, "is_null") == 0) {
        if (node->data.function_call.arg_count > 0) {
            Value* arg = interpreter_eval(interp, node->data.function_call.args[0]);
            int result = (arg->type == VAL_NULL);
            value_free(arg);
            return value_create_number(result);
        }
        return value_create_number(0);
    }
    
    if (strcmp(name, "is_number") == 0) {
        if (node->data.function_call.arg_count > 0) {
            Value* arg = interpreter_eval(interp, node->data.function_call.args[0]);
            int result = (arg->type == VAL_NUMBER);
            value_free(arg);
            return value_create_number(result);
        }
        return value_create_number(0);
    }
    
    if (strcmp(name, "is_string") == 0) {
        if (node->data.function_call.arg_count > 0) {
            Value* arg = interpreter_eval(interp, node->data.function_call.args[0]);
            int result = (arg->type == VAL_STRING);
            value_free(arg);
            return value_create_number(result);
        }
        return value_create_number(0);
    }
    
    if (strcmp(name, "is_bool") == 0) {
        if (node->data.function_call.arg_count > 0) {
            Value* arg = interpreter_eval(interp, node->data.function_call.args[0]);
            // FineLang에서 boolean은 숫자로 표현 (0 또는 1)
            int result = (arg->type == VAL_NUMBER && 
                         (arg->data.number == 0.0 || arg->data.number == 1.0));
            value_free(arg);
            return value_create_number(result);
        }
        return value_create_number(0);
    }
    
    if (strcmp(name, "is_array") == 0) {
        if (node->data.function_call.arg_count > 0) {
            Value* arg = interpreter_eval(interp, node->data.function_call.args[0]);
            int result = (arg->type == VAL_ARRAY);
            value_free(arg);
            return value_create_number(result);
        }
        return value_create_number(0);
    }
    
    if (strcmp(name, "is_dict") == 0) {
        if (node->data.function_call.arg_count > 0) {
            Value* arg = interpreter_eval(interp, node->data.function_call.args[0]);
            int result = (arg->type == VAL_DICT);
            value_free(arg);
            return value_create_number(result);
        }
        return value_create_number(0);
    }
    
    if (strcmp(name, "is_matrix") == 0) {
        if (node->data.function_call.arg_count > 0) {
            Value* arg = interpreter_eval(interp, node->data.function_call.args[0]);
            int result = (arg->type == VAL_MATRIX);
            value_free(arg);
            return value_create_number(result);
        }
        return value_create_number(0);
    }
    
    if (strcmp(name, "typeof") == 0) {
        if (node->data.function_call.arg_count > 0) {
            Value* arg = interpreter_eval(interp, node->data.function_call.args[0]);
            char* type_name = "";
            
            switch (arg->type) {
                case VAL_NULL:      type_name = "null"; break;
                case VAL_NUMBER:    type_name = "number"; break;
                case VAL_STRING:    type_name = "string"; break;
                case VAL_ARRAY:     type_name = "array"; break;
                case VAL_DICT:      type_name = "dict"; break;
                case VAL_MATRIX:    type_name = "matrix"; break;
                case VAL_FUNCTION:  type_name = "function"; break;
                case VAL_CLASS:     type_name = "class"; break;
                case VAL_INSTANCE:  type_name = "instance"; break;
                case VAL_MODULE:    type_name = "module"; break;
                case VAL_EXCEPTION: type_name = "exception"; break;
                default:            type_name = "unknown"; break;
            }
            
            value_free(arg);
            return value_create_string(type_name);
        }
        return value_create_string("undefined");
    }
    
    // map 함수: map(function, array)
    // map 함수: map(function, array)
    if (strcmp(name, "map") == 0) {
        if (node->data.function_call.arg_count >= 2) {
            Value* func = interpreter_eval(interp, node->data.function_call.args[0]);
            Value* arr = interpreter_eval(interp, node->data.function_call.args[1]);
            
            if (func->type == VAL_FUNCTION && arr->type == VAL_ARRAY) {
                Value** new_elements = (Value**)malloc(sizeof(Value*) * arr->data.array.count);
                
                for (int i = 0; i < arr->data.array.count; i++) {
                    // 함수 호출을 위한 임시 AST 노드 생성
                    ASTNode** args = (ASTNode**)malloc(sizeof(ASTNode*) * 1);
                    args[0] = (ASTNode*)malloc(sizeof(ASTNode));
                    args[0]->type = AST_NUMBER;
                    args[0]->data.number = arr->data.array.elements[i]->data.number;
                    args[0]->line = node->line;
                    
                    // 함수 환경 생성 및 매개변수 바인딩
                    Environment* func_env = environment_create(func->data.function.closure);
                    environment_set(func_env, func->data.function.params[0], arr->data.array.elements[i]);
                    
                    Environment* prev_env = interp->current_env;
                    interp->current_env = func_env;
                    interp->has_returned = 0;
                    
                    Value* result = interpreter_eval(interp, func->data.function.body);
                    
                    if (interp->has_returned) {
                        new_elements[i] = value_copy(interp->return_value);
                        value_free(interp->return_value);
                        interp->return_value = NULL;
                        interp->has_returned = 0;
                        value_free(result);
                    } else {
                        new_elements[i] = result;
                    }
                    
                    interp->current_env = prev_env;
                    environment_free(func_env);
                    free(args[0]);
                    free(args);
                }
                
                Value* result = value_create_array(new_elements, arr->data.array.count);
                value_free(func);
                value_free(arr);
                return result;
            }
            value_free(func);
            value_free(arr);
        }
        return value_create_null();
    }
    
    // filter 함수: filter(predicate, array)
    if (strcmp(name, "filter") == 0) {
        if (node->data.function_call.arg_count >= 2) {
            Value* func = interpreter_eval(interp, node->data.function_call.args[0]);
            Value* arr = interpreter_eval(interp, node->data.function_call.args[1]);
            
            if (func->type == VAL_FUNCTION && arr->type == VAL_ARRAY) {
                Value** temp_elements = (Value**)malloc(sizeof(Value*) * arr->data.array.count);
                int count = 0;
                
                for (int i = 0; i < arr->data.array.count; i++) {
                    // 함수 환경 생성 및 매개변수 바인딩
                    Environment* func_env = environment_create(func->data.function.closure);
                    environment_set(func_env, func->data.function.params[0], arr->data.array.elements[i]);
                    
                    Environment* prev_env = interp->current_env;
                    interp->current_env = func_env;
                    interp->has_returned = 0;
                    
                    Value* result = interpreter_eval(interp, func->data.function.body);
                    
                    int keep = 0;
                    if (interp->has_returned) {
                        keep = (interp->return_value->type == VAL_NUMBER && interp->return_value->data.number != 0);
                        value_free(interp->return_value);
                        interp->return_value = NULL;
                        interp->has_returned = 0;
                        value_free(result);
                    } else {
                        keep = (result->type == VAL_NUMBER && result->data.number != 0);
                        value_free(result);
                    }
                    
                    if (keep) {
                        temp_elements[count++] = value_copy(arr->data.array.elements[i]);
                    }
                    
                    interp->current_env = prev_env;
                    environment_free(func_env);
                }
                
                Value** new_elements = (Value**)malloc(sizeof(Value*) * count);
                for (int i = 0; i < count; i++) {
                    new_elements[i] = temp_elements[i];
                }
                free(temp_elements);
                
                Value* result = value_create_array(new_elements, count);
                value_free(func);
                value_free(arr);
                return result;
            }
            value_free(func);
            value_free(arr);
        }
        return value_create_null();
    }
    
    // reduce 함수: reduce(function, array, initial)
    if (strcmp(name, "reduce") == 0) {
        if (node->data.function_call.arg_count >= 3) {
            Value* func = interpreter_eval(interp, node->data.function_call.args[0]);
            Value* arr = interpreter_eval(interp, node->data.function_call.args[1]);
            Value* initial = interpreter_eval(interp, node->data.function_call.args[2]);
            
            if (func->type == VAL_FUNCTION && arr->type == VAL_ARRAY) {
                Value* accumulator = value_copy(initial);
                
                for (int i = 0; i < arr->data.array.count; i++) {
                    // 함수 환경 생성 및 매개변수 바인딩
                    Environment* func_env = environment_create(func->data.function.closure);
                    environment_set(func_env, func->data.function.params[0], accumulator);
                    environment_set(func_env, func->data.function.params[1], arr->data.array.elements[i]);
                    
                    Environment* prev_env = interp->current_env;
                    interp->current_env = func_env;
                    interp->has_returned = 0;
                    
                    Value* result = interpreter_eval(interp, func->data.function.body);
                    
                    value_free(accumulator);
                    if (interp->has_returned) {
                        accumulator = value_copy(interp->return_value);
                        value_free(interp->return_value);
                        interp->return_value = NULL;
                        interp->has_returned = 0;
                        value_free(result);
                    } else {
                        accumulator = result;
                    }
                    
                    interp->current_env = prev_env;
                    environment_free(func_env);
                }
                
                value_free(func);
                value_free(arr);
                value_free(initial);
                return accumulator;
            }
            value_free(func);
            value_free(arr);
            value_free(initial);
        }
        return value_create_null();
    }
    
    // 사용자 정의 함수
    Value* func = environment_get(interp->current_env, name);
    if (func && func->type == VAL_FUNCTION) {
        // 스택에 함수 호출 정보 추가
        Value* stack_error = stack_push(interp, name, interp->current_file, node->line);
        if (stack_error) {
            interp->current_exception = stack_error;
            interp->has_exception = 1;
            return value_create_null();  // RecursionError 발생
        }
        
        Environment* func_env = environment_create(func->data.function.closure);
        
        // 매개변수 바인딩
        for (int i = 0; i < func->data.function.param_count; i++) {
            Value* arg = (i < node->data.function_call.arg_count) ?
                interpreter_eval(interp, node->data.function_call.args[i]) :
                value_create_null();
            environment_set(func_env, func->data.function.params[i], arg);
        }
        
        Environment* prev_env = interp->current_env;
        interp->current_env = func_env;
        interp->has_returned = 0;
        
        Value* result = interpreter_eval(interp, func->data.function.body);
        
        if (interp->has_returned) {
            value_free(result);
            result = value_copy(interp->return_value);
            value_free(interp->return_value);
            interp->return_value = NULL;
            interp->has_returned = 0;
        }
        
        interp->current_env = prev_env;
        environment_free(func_env);
        
        // 스택에서 함수 호출 정보 제거
        stack_pop(interp);
        
        return result;
    }
    
    return value_create_null();
}

// 환경 생성
Environment* environment_create(Environment* parent) {
    Environment* env = (Environment*)malloc(sizeof(Environment));
    env->capacity = 10;
    env->names = (char**)malloc(sizeof(char*) * env->capacity);
    env->values = (Value**)malloc(sizeof(Value*) * env->capacity);
    env->count = 0;
    env->parent = parent;
    return env;
}

// 환경 메모리 해제
void environment_free(Environment* env) {
    if (!env) return;
    for (int i = 0; i < env->count; i++) {
        free(env->names[i]);
        value_free(env->values[i]);
    }
    free(env->names);
    free(env->values);
    free(env);
}

// 변수 설정
void environment_set(Environment* env, char* name, Value* value) {
    for (int i = 0; i < env->count; i++) {
        if (strcmp(env->names[i], name) == 0) {
            value_free(env->values[i]);
            env->values[i] = value;
            return;
        }
    }
    
    if (env->count >= env->capacity) {
        env->capacity *= 2;
        env->names = (char**)realloc(env->names, sizeof(char*) * env->capacity);
        env->values = (Value**)realloc(env->values, sizeof(Value*) * env->capacity);
    }
    
    env->names[env->count] = strdup(name);
    env->values[env->count] = value;
    env->count++;
}

// 변수 가져오기
Value* environment_get(Environment* env, char* name) {
    for (int i = 0; i < env->count; i++) {
        if (strcmp(env->names[i], name) == 0) {
            return env->values[i];
        }
    }
    
    if (env->parent) {
        return environment_get(env->parent, name);
    }
    
    return NULL;
}

// 값 생성 함수들
Value* value_create_number(double num) {
    Value* val = (Value*)malloc(sizeof(Value));
    val->type = VAL_NUMBER;
    val->data.number = num;
    return val;
}

Value* value_create_string(char* str) {
    Value* val = (Value*)malloc(sizeof(Value));
    val->type = VAL_STRING;
    val->data.string = strdup(str);
    return val;
}

Value* value_create_array(Value** elements, int count) {
    Value* val = (Value*)malloc(sizeof(Value));
    val->type = VAL_ARRAY;
    val->data.array.elements = elements;
    val->data.array.count = count;
    return val;
}

Value* value_create_dict(char** keys, Value** values, int count) {
    Value* val = (Value*)malloc(sizeof(Value));
    val->type = VAL_DICT;
    val->data.dict.keys = keys;
    val->data.dict.values = values;
    val->data.dict.count = count;
    return val;
}

Value* value_create_null() {
    Value* val = (Value*)malloc(sizeof(Value));
    val->type = VAL_NULL;
    return val;
}

Value* value_create_exception(char* type, char* message) {
    Value* val = (Value*)malloc(sizeof(Value));
    val->type = VAL_EXCEPTION;
    val->data.exception.type = strdup(type);
    val->data.exception.message = strdup(message);
    val->data.exception.stack_trace = NULL;
    val->data.exception.stack_depth = 0;
    return val;
}

Value* value_create_module(char* name, Environment* exports) {
    Value* val = (Value*)malloc(sizeof(Value));
    val->type = VAL_MODULE;
    val->data.module.name = strdup(name);
    val->data.module.exports = exports;
    return val;
}

Value* value_create_matrix(int rows, int cols) {
    Value* val = (Value*)malloc(sizeof(Value));
    val->type = VAL_MATRIX;
    val->data.matrix.rows = rows;
    val->data.matrix.cols = cols;
    
    // 2차원 배열 할당
    val->data.matrix.data = (double**)malloc(sizeof(double*) * rows);
    for (int i = 0; i < rows; i++) {
        val->data.matrix.data[i] = (double*)calloc(cols, sizeof(double));
    }
    
    return val;
}

// 값 복사
Value* value_copy(Value* val) {
    if (!val) return value_create_null();
    
    switch (val->type) {
        case VAL_NUMBER:
            return value_create_number(val->data.number);
        case VAL_STRING:
            return value_create_string(val->data.string);
        case VAL_ARRAY: {
            Value** elements = (Value**)malloc(sizeof(Value*) * val->data.array.count);
            for (int i = 0; i < val->data.array.count; i++) {
                elements[i] = value_copy(val->data.array.elements[i]);
            }
            return value_create_array(elements, val->data.array.count);
        }
        case VAL_DICT: {
            char** keys = (char**)malloc(sizeof(char*) * val->data.dict.count);
            Value** values = (Value**)malloc(sizeof(Value*) * val->data.dict.count);
            for (int i = 0; i < val->data.dict.count; i++) {
                keys[i] = strdup(val->data.dict.keys[i]);
                values[i] = value_copy(val->data.dict.values[i]);
            }
            return value_create_dict(keys, values, val->data.dict.count);
        }
        case VAL_FUNCTION: {
            // 함수는 참조로 전달 (클로저를 공유)
            return val;
        }
        case VAL_CLASS:
        case VAL_INSTANCE:
        case VAL_MODULE:
            // 클래스, 인스턴스, 모듈은 참조로 전달 (간단한 구현)
            return val;
        case VAL_MATRIX: {
            Value* copy = value_create_matrix(val->data.matrix.rows, val->data.matrix.cols);
            for (int i = 0; i < val->data.matrix.rows; i++) {
                for (int j = 0; j < val->data.matrix.cols; j++) {
                    copy->data.matrix.data[i][j] = val->data.matrix.data[i][j];
                }
            }
            return copy;
        }
        case VAL_EXCEPTION: {
            Value* exc = value_create_exception(val->data.exception.type, val->data.exception.message);
            exc->data.exception.stack_trace = stack_copy(val->data.exception.stack_trace);
            exc->data.exception.stack_depth = val->data.exception.stack_depth;
            return exc;
        }
        default:
            return value_create_null();
    }
}

// 값 메모리 해제
void value_free(Value* val) {
    // 간단하게 하기 위해 메모리 해제를 비활성화
    // 짧은 실행 시간의 스크립트에서는 OS가 정리함
    return;
    
    if (!val) return;
    
    switch (val->type) {
        case VAL_STRING:
            free(val->data.string);
            break;
        case VAL_ARRAY:
            for (int i = 0; i < val->data.array.count; i++) {
                value_free(val->data.array.elements[i]);
            }
            free(val->data.array.elements);
            break;
        case VAL_DICT:
            for (int i = 0; i < val->data.dict.count; i++) {
                free(val->data.dict.keys[i]);
                value_free(val->data.dict.values[i]);
            }
            free(val->data.dict.keys);
            free(val->data.dict.values);
            break;
        case VAL_FUNCTION:
            // 함수의 파라미터 메모리 해제
            for (int i = 0; i < val->data.function.param_count; i++) {
                free(val->data.function.params[i]);
            }
            free(val->data.function.params);
            // body는 AST 노드이므로 여기서 해제하지 않음 (AST에서 관리)
            break;
        case VAL_CLASS:
            free(val->data.class_def.name);
            if (val->data.class_def.parent_class) {
                free(val->data.class_def.parent_class);
            }
            for (int i = 0; i < val->data.class_def.field_count; i++) {
                free(val->data.class_def.fields[i]);
            }
            free(val->data.class_def.fields);
            free(val->data.class_def.methods);
            break;
        case VAL_INSTANCE:
            free(val->data.instance.class_name);
            if (val->data.instance.parent_class) {
                free(val->data.instance.parent_class);
            }
            for (int i = 0; i < val->data.instance.field_count; i++) {
                free(val->data.instance.field_names[i]);
                value_free(val->data.instance.field_values[i]);
            }
            free(val->data.instance.field_names);
            free(val->data.instance.field_values);
            break;
        case VAL_EXCEPTION:
            free(val->data.exception.type);
            free(val->data.exception.message);
            if (val->data.exception.stack_trace) {
                stack_frame_free(val->data.exception.stack_trace);
            }
            break;
        case VAL_MATRIX:
            for (int i = 0; i < val->data.matrix.rows; i++) {
                free(val->data.matrix.data[i]);
            }
            free(val->data.matrix.data);
            break;
        default:
            break;
    }
    
    free(val);
}

// 값 출력
void value_print(Value* val) {
    if (!val) {
        printf("null");
        return;
    }
    
    switch (val->type) {
        case VAL_NUMBER:
            if (val->data.number == (int)val->data.number) {
                printf("%d", (int)val->data.number);
            } else {
                printf("%g", val->data.number);
            }
            break;
        case VAL_STRING:
            printf("%s", val->data.string);
            break;
        case VAL_ARRAY:
            printf("[");
            for (int i = 0; i < val->data.array.count; i++) {
                value_print(val->data.array.elements[i]);
                if (i < val->data.array.count - 1) printf(", ");
            }
            printf("]");
            break;
        case VAL_DICT:
            printf("{");
            for (int i = 0; i < val->data.dict.count; i++) {
                printf("%s: ", val->data.dict.keys[i]);
                value_print(val->data.dict.values[i]);
                if (i < val->data.dict.count - 1) printf(", ");
            }
            printf("}");
            break;
        case VAL_CLASS:
            printf("<class %s>", val->data.class_def.name);
            break;
        case VAL_INSTANCE:
            printf("<%s instance>", val->data.instance.class_name);
            break;
        case VAL_FUNCTION:
            printf("<function>");
            break;
        case VAL_MODULE:
            printf("<module '%s'>", val->data.module.name);
            break;
        case VAL_MATRIX:
            printf("Matrix(%dx%d)[\n", val->data.matrix.rows, val->data.matrix.cols);
            for (int i = 0; i < val->data.matrix.rows; i++) {
                printf("  [");
                for (int j = 0; j < val->data.matrix.cols; j++) {
                    printf("%g", val->data.matrix.data[i][j]);
                    if (j < val->data.matrix.cols - 1) printf(", ");
                }
                printf("]");
                if (i < val->data.matrix.rows - 1) printf(",");
                printf("\n");
            }
            printf("]");
            break;
        case VAL_EXCEPTION:
            // 스택 트레이스가 있으면 먼저 출력
            if (val->data.exception.stack_trace) {
                print_stack_trace(val->data.exception.stack_trace, val->data.exception.stack_depth);
            }
            printf("%s: %s", val->data.exception.type, val->data.exception.message);
            break;
        case VAL_NULL:
            printf("null");
            break;
        default:
            printf("unknown");
            break;
    }
}

// ============ 스택 트레이스 함수들 ============

// 스택 프레임 생성
StackFrame* stack_frame_create(char* function_name, char* file_name, int line_number) {
    StackFrame* frame = (StackFrame*)malloc(sizeof(StackFrame));
    frame->function_name = function_name ? strdup(function_name) : strdup("<module>");
    frame->file_name = file_name ? strdup(file_name) : strdup("<unknown>");
    frame->line_number = line_number;
    frame->next = NULL;
    return frame;
}

// 스택 프레임 해제
void stack_frame_free(StackFrame* frame) {
    if (!frame) return;
    
    StackFrame* current = frame;
    while (current) {
        StackFrame* next = current->next;
        free(current->function_name);
        free(current->file_name);
        free(current);
        current = next;
    }
}

// 스택에 프레임 push
Value* stack_push(Interpreter* interp, char* function_name, char* file_name, int line_number) {
    // 스택 깊이 체크
    if (interp->stack_depth >= interp->max_stack_depth) {
        // RecursionError 발생
        Value* err = value_create_exception(
            "RecursionError",
            "maximum recursion depth exceeded"
        );
        exception_attach_stack_trace(interp, err);
        return err;
    }
    
    StackFrame* frame = stack_frame_create(function_name, file_name, line_number);
    frame->next = interp->call_stack;
    interp->call_stack = frame;
    interp->stack_depth++;
    return NULL;  // 성공
}

// 스택에서 프레임 pop
void stack_pop(Interpreter* interp) {
    if (!interp->call_stack) return;
    
    StackFrame* frame = interp->call_stack;
    interp->call_stack = frame->next;
    frame->next = NULL;
    stack_frame_free(frame);
    interp->stack_depth--;
}

// 스택 복사 (예외에 첨부하기 위해)
StackFrame* stack_copy(StackFrame* stack) {
    if (!stack) return NULL;
    
    StackFrame* new_head = NULL;
    StackFrame* new_tail = NULL;
    StackFrame* current = stack;
    
    while (current) {
        StackFrame* frame = stack_frame_create(
            current->function_name,
            current->file_name,
            current->line_number
        );
        
        if (!new_head) {
            new_head = frame;
            new_tail = frame;
        } else {
            new_tail->next = frame;
            new_tail = frame;
        }
        
        current = current->next;
    }
    
    return new_head;
}

// 스택 트레이스 출력
void print_stack_trace(StackFrame* stack, int depth) {
    if (!stack) return;
    
    printf("\nTraceback (most recent call last):\n");
    
    StackFrame* current = stack;
    int count = 0;
    
    while (current && count < depth) {
        printf("  File \"%s\", line %d, in %s\n",
               current->file_name,
               current->line_number,
               current->function_name);
        current = current->next;
        count++;
    }
    
    if (current) {
        printf("  ... (%d more frames)\n", depth - count);
    }
}

// 예외에 스택 트레이스 첨부
void exception_attach_stack_trace(Interpreter* interp, Value* exception) {
    if (!exception || exception->type != VAL_EXCEPTION) return;
    
    // 기존 스택 트레이스가 있으면 해제
    if (exception->data.exception.stack_trace) {
        stack_frame_free(exception->data.exception.stack_trace);
    }
    
    // 현재 스택 복사
    exception->data.exception.stack_trace = stack_copy(interp->call_stack);
    exception->data.exception.stack_depth = interp->stack_depth;
}
