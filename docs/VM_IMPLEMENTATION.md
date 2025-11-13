# FineLang 바이트코드 VM 구현 문서

> **버전: v2.3.0** | 스택 기반 가상 머신 구현 상세 문서

---

## 📋 목차

1. [개요](#개요)
2. [아키텍처](#아키텍처)
3. [OpCode 명령어 세트](#opcode-명령어-세트)
4. [바이트코드 구조](#바이트코드-구조)
5. [VM 실행 엔진](#vm-실행-엔진)
6. [컴파일러](#컴파일러)
7. [구현된 기능](#구현된-기능)
8. [성능 특성](#성능-특성)
9. [제한사항](#제한사항)
10. [향후 계획](#향후-계획)

---

## 개요

FineLang v2.3.0은 스택 기반 바이트코드 가상 머신을 도입하여 성능과 메모리 효율성을 대폭 향상시켰습니다.

### 주요 특징

- 📦 **40+ OpCode**: 효율적인 명령어 세트
- 🔧 **스택 기반**: 256 크기의 Value 스택
- 🎯 **최적화된 점프**: if/while/for 제어문
- 💾 **상수 풀**: 리터럴 값 중복 제거
- 🚀 **디스어셈블러**: 디버깅을 위한 바이트코드 출력

### 성능 이점

| 항목 | 인터프리터 | 바이트코드 VM |
|------|----------|--------------|
| 파싱 | 매번 | 1회 |
| AST 순회 | 매번 | 없음 |
| 명령어 디스패치 | 간접 | 직접 |
| 메모리 사용 | 높음 | 낮음 |

---

## 아키텍처

```
┌─────────────────────────────────────────────────────────────┐
│                      FineLang Source Code                    │
└──────────────────────────┬──────────────────────────────────┘
                           │
                           ▼
                    ┌──────────────┐
                    │    Lexer     │  토큰화
                    └──────┬───────┘
                           │
                           ▼
                    ┌──────────────┐
                    │    Parser    │  AST 생성
                    └──────┬───────┘
                           │
                           ▼
                    ┌──────────────┐
                    │   Compiler   │  바이트코드 생성
                    └──────┬───────┘
                           │
                           ▼
              ┌────────────────────────┐
              │   BytecodeChunk        │
              │  - Instructions        │
              │  - Constants Pool      │
              └────────┬───────────────┘
                       │
                       ▼
              ┌────────────────────────┐
              │    Virtual Machine     │
              │  - Stack (256)         │
              │  - Instruction Pointer │
              │  - Environment         │
              └────────────────────────┘
```

---

## OpCode 명령어 세트

### 상수 로드

| OpCode | 설명 | 스택 변화 |
|--------|------|-----------|
| `OP_LOAD_CONST` | 상수 풀에서 값 로드 | → value |
| `OP_LOAD_TRUE` | true 푸시 | → true |
| `OP_LOAD_FALSE` | false 푸시 | → false |
| `OP_LOAD_NULL` | null 푸시 | → null |

### 변수 연산

| OpCode | 설명 | 스택 변화 |
|--------|------|-----------|
| `OP_LOAD_VAR` | 변수 값 로드 | → value |
| `OP_STORE_VAR` | 변수에 값 저장 | value → value |

### 산술 연산

| OpCode | 설명 | 스택 변화 |
|--------|------|-----------|
| `OP_ADD` | 덧셈 | a, b → (a+b) |
| `OP_SUBTRACT` | 뺄셈 | a, b → (a-b) |
| `OP_MULTIPLY` | 곱셈 | a, b → (a*b) |
| `OP_DIVIDE` | 나눗셈 | a, b → (a/b) |
| `OP_MODULO` | 나머지 | a, b → (a%b) |
| `OP_FLOOR_DIV` | 몫 | a, b → (a//b) |
| `OP_NEGATE` | 부호 반전 | a → (-a) |

### 비교 연산

| OpCode | 설명 | 스택 변화 |
|--------|------|-----------|
| `OP_EQUAL` | 같음 | a, b → (a==b) |
| `OP_NOT_EQUAL` | 다름 | a, b → (a!=b) |
| `OP_LESS` | 작음 | a, b → (a<b) |
| `OP_LESS_EQUAL` | 작거나 같음 | a, b → (a<=b) |
| `OP_GREATER` | 큼 | a, b → (a>b) |
| `OP_GREATER_EQUAL` | 크거나 같음 | a, b → (a>=b) |

### 논리 연산

| OpCode | 설명 | 스택 변화 |
|--------|------|-----------|
| `OP_NOT` | 논리 부정 | a → (!a) |

### 배열/딕셔너리

| OpCode | 설명 | 스택 변화 |
|--------|------|-----------|
| `OP_BUILD_ARRAY` | 배열 생성 | v1..vn → [v1..vn] |
| `OP_BUILD_DICT` | 딕셔너리 생성 | k1,v1..kn,vn → {k1:v1..} |
| `OP_INDEX` | 인덱스 접근 | array, idx → value |
| `OP_STORE_INDEX` | 인덱스에 저장 | array, idx, val → |
| `OP_ARRAY_LENGTH` | 배열 길이 | array → length |

### 제어 흐름

| OpCode | 설명 | 설명 |
|--------|------|------|
| `OP_JUMP` | 무조건 점프 | PC = target |
| `OP_JUMP_IF_FALSE` | false면 점프 | if !cond: PC = target |
| `OP_JUMP_IF_TRUE` | true면 점프 | if cond: PC = target |

### 기타

| OpCode | 설명 | 스택 변화 |
|--------|------|-----------|
| `OP_PRINT` | 값 출력 | value → |
| `OP_POP` | 스택 팝 | value → |
| `OP_DUP` | 스택 복제 | value → value, value |
| `OP_HALT` | VM 정지 | - |

---

## 바이트코드 구조

### BytecodeChunk

```c
typedef struct {
    Instruction* instructions;    // 명령어 배열
    int count;                     // 명령어 수
    int capacity;                  // 할당된 용량
    
    Value** constants;             // 상수 풀
    int constant_count;            // 상수 개수
    int constant_capacity;         // 상수 용량
} BytecodeChunk;
```

### Instruction

```c
typedef struct {
    OpCode opcode;                 // 명령어 코드
    union {
        int64_t int_operand;       // 정수 피연산자
        double float_operand;      // 실수 피연산자
    } operand;
} Instruction;
```

---

## VM 실행 엔진

### VM 구조

```c
typedef struct {
    BytecodeChunk* chunk;          // 실행할 바이트코드
    Instruction* ip;               // 명령어 포인터
    Value* stack[256];             // Value 스택
    int stack_top;                 // 스택 포인터
    Environment* globals;          // 전역 변수 환경
} VM;
```

### 실행 루프

```c
VMResult vm_run(VM* vm) {
    while (1) {
        Instruction* instr = vm->ip++;
        
        switch (instr->opcode) {
            case OP_LOAD_CONST: {
                int index = instr->operand.int_operand;
                Value* constant = chunk->constants[index];
                vm_push(vm, value_copy(constant));
                break;
            }
            
            case OP_ADD: {
                Value* right = vm_pop(vm);
                Value* left = vm_pop(vm);
                // ... 덧셈 로직
                vm_push(vm, result);
                break;
            }
            
            // ... 다른 OpCode 처리
        }
    }
}
```

### 안전성 기능

- **스택 오버플로우/언더플로우 검사**
- **명령어 카운트 제한** (무한 루프 방지)
- **배열 인덱스 범위 검사**
- **타입 검사**

---

## 컴파일러

### AST → 바이트코드 변환

```c
void compile_expression(Compiler* compiler, ASTNode* node) {
    switch (node->type) {
        case AST_NUMBER: {
            Value* num = value_create_number(node->data.number);
            int index = bytecode_add_constant(compiler->chunk, num);
            bytecode_emit_with_operand(compiler->chunk, OP_LOAD_CONST, index);
            break;
        }
        
        case AST_BINARY_OP: {
            compile_expression(compiler, node->data.binary_op.left);
            compile_expression(compiler, node->data.binary_op.right);
            
            switch (node->data.binary_op.operator) {
                case '+': bytecode_emit(compiler->chunk, OP_ADD); break;
                case '-': bytecode_emit(compiler->chunk, OP_SUBTRACT); break;
                // ...
            }
            break;
        }
    }
}
```

### 제어문 컴파일

**If 문:**
```
condition
JUMP_IF_FALSE end
then_branch
end:
```

**While 문:**
```
loop_start:
condition
JUMP_IF_FALSE end
body
JUMP loop_start
end:
```

**For 문:**
```
iterable → __tmp_array__
0 → __tmp_idx__
loop_start:
__tmp_idx__ < ARRAY_LENGTH(__tmp_array__)
JUMP_IF_FALSE end
__tmp_array__[__tmp_idx__] → iterator
body
__tmp_idx__ = __tmp_idx__ + 1
JUMP loop_start
end:
```

---

## 구현된 기능

### ✅ 완료된 기능

- [x] 기본 산술 연산 (+, -, *, /, %, //)
- [x] 비교 연산 (==, !=, <, <=, >, >=)
- [x] 논리 연산 (!)
- [x] 변수 선언/할당 (let, =)
- [x] If/Else 조건문
- [x] While 루프
- [x] For 루프 (for item in array)
- [x] 배열 생성 및 인덱싱
- [x] Print 함수
- [x] Boolean 타입 (true/false)
- [x] 주석 처리 (//, #)
- [x] 상수 풀 최적화
- [x] 바이트코드 디스어셈블러

### ❌ 미구현 기능

- [ ] 함수 정의/호출
- [ ] 클래스 시스템
- [ ] 딕셔너리 연산
- [ ] 예외 처리
- [ ] 모듈 시스템
- [ ] 고차 함수 (map, filter, reduce)

---

## 성능 특성

### 메모리 사용

```
기본 VM 크기: ~2KB
- BytecodeChunk: 동적 할당
- 스택: 256 * sizeof(Value*) = 2KB
- Environment: 동적 할당
```

### 실행 제한

- **최대 명령어 실행**: 10,000회 (무한 루프 방지)
- **스택 크기**: 256
- **상수 풀**: 동적 확장 (제한 없음)

### 최적화 기법

1. **상수 풀링**: 리터럴 값 중복 제거
2. **직접 점프**: if/while/for에서 효율적인 분기
3. **스택 기반**: 임시 변수 최소화
4. **타입 검사 캐싱**: 반복 검사 최소화

---

## 제한사항

### 현재 제한

1. **함수 미지원**: 함수 정의/호출 미구현
2. **클래스 미지원**: OOP 기능 미구현
3. **배열 크기**: for 루프에서 배열 길이 사용
4. **스택 크기**: 256으로 고정
5. **재귀 제한**: 스택 크기로 인한 제한

### 알려진 이슈

- For 루프는 배열만 지원 (딕셔너리, 문자열 미지원)
- 중첩 for 루프는 별도 임시 변수 필요
- 메모리 누수 가능성 (value_free 비활성화)

---

## 향후 계획

### v2.4.0 - 함수 지원

```finelang
fn add(a, b) {
    return a + b
}

print(add(10, 20))  # 30
```

**구현 계획:**
- `OP_CALL`, `OP_RETURN` OpCode 추가
- 호출 프레임 스택
- 클로저 지원

### v2.5.0 - 클래스 지원

```finelang
class Point {
    fn init(x, y) {
        this.x = x
        this.y = y
    }
}

let p = new Point(10, 20)
```

**구현 계획:**
- `OP_GET_PROPERTY`, `OP_SET_PROPERTY`
- 인스턴스 생성 OpCode
- 메서드 호출 최적화

### v2.6.0 - 최적화

- **상수 폴딩**: 컴파일 타임 계산
- **데드 코드 제거**: 사용되지 않는 코드 제거
- **인라이닝**: 작은 함수 인라인
- **레지스터 할당**: 스택 사용 최소화

---

## 테스트

### 테스트 파일

```bash
examples/test_vm_simple.fine    # 기본 산술, Boolean
examples/test_vm_vars.fine      # 변수, if/else
examples/test_vm_while.fine     # while 루프
examples/test_vm_for.fine       # for 루프
examples/test_vm_for_simple.fine # for 루프 + 변수
```

### 실행 방법

```bash
# 빌드
make vm_test

# 테스트 실행
./vm_test examples/test_vm_for.fine

# 출력:
# === Lexing ===
# === Parsing ===
# === Compiling to bytecode ===
# === Bytecode Disassembly ===
# 0000  LOAD_CONST  0 (1)
# ...
# === VM Execution ===
# 1
# 2
# 3
# 4
# 5
```

---

## 참고 자료

### 코드 파일

- `src/bytecode.h/c` - 바이트코드 시스템
- `src/compiler.h/c` - AST → 바이트코드 컴파일러
- `src/vm.h/c` - 가상 머신 실행 엔진
- `src/vm_test.c` - VM 테스트 도구

### 관련 문서

- [README.md](README.md) - 프로젝트 개요
- [SYNTAX_GUIDE.md](SYNTAX_GUIDE.md) - 문법 가이드
- [CHANGELOG.md](CHANGELOG.md) - 변경 이력

---

## 기여

바이트코드 VM 개선에 기여하고 싶으시다면:

1. 이슈 생성 (버그 리포트, 기능 제안)
2. Pull Request (코드 개선, 새 OpCode)
3. 테스트 케이스 추가
4. 문서 개선

---

**Made with ❤️ for FineLang**

*Last updated: 2025-11-12*
