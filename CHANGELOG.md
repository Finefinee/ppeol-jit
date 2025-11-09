# FineLang 변경 이력

## v2.2.1 (2025-01-09) - 2차원 인덱싱 지원 🎯

### 새로운 기능

#### 2차원 인덱싱
- ✨ **m[i][j] 문법**: 행렬의 개별 요소에 직접 접근
  ```finelang
  let M = [[1, 2, 3], [4, 5, 6]]
  print(M[0][0])  # 1
  print(M[1][2])  # 6
  ```

- ✨ **연속 인덱싱**: parser_parse_postfix의 while 루프가 자동 지원
  ```finelang
  let M = [[1, 2], [3, 4]]
  let val = M[0][1]  # 2
  ```

- ✨ **행렬 순회**: 중첩 루프로 모든 요소 접근 가능
  ```finelang
  let i = 0
  while i < rows {
      let j = 0
      while j < cols {
          print(M[i][j])
          j = j + 1
      }
      i = i + 1
  }
  ```

### 구현 세부사항
- **파서**: parser_parse_postfix()의 while 루프가 연속 인덱싱 자동 처리
- **인터프리터**: m[i]가 배열 반환, m[i][j]는 배열[j] 접근

### 파일 변경
- **수정**: `src/parser.c` - AST_INDEX 노드에 line 번호 추가
- **수정**: `SYNTAX_GUIDE.md` - 2차원 인덱싱 예제 추가
- **신규**: `test_simple_2d.fine`, `test_2d_index.fine` - 테스트 파일

### 성과
- 🎯 **완전한 다차원 배열 지원**
- 🎯 **직관적인 m[i][j] 문법**
- 🎯 **NumPy/MATLAB 스타일 인덱싱**

## v2.2.0 (2025-01-09) - 행렬 연산 완성 📐

### 새로운 기능

#### 행렬 사칙연산
- ✨ **행렬 덧셈 (+)**: 같은 크기의 행렬 요소별 덧셈
  ```finelang
  let A = [[1, 2], [3, 4]]
  let B = [[5, 6], [7, 8]]
  let C = A + B  // [[6, 8], [10, 12]]
  ```

- ✨ **행렬 뺄셈 (-)**: 같은 크기의 행렬 요소별 뺄셈
  ```finelang
  let D = B - A  // [[4, 4], [4, 4]]
  ```

- ✨ **스칼라 곱셈 (*)**: 행렬과 숫자의 곱셈 (교환법칙 지원)
  ```finelang
  let E = A * 2   // [[2, 4], [6, 8]]
  let F = 3 * B   // [[15, 18], [21, 24]]
  ```

- ✨ **행렬 곱셈 (@)**: 표준 행렬 곱셈 (NumPy/MATLAB 스타일)
  ```finelang
  let M1 = [[1, 2], [3, 4]]
  let M2 = [[2, 0], [1, 2]]
  let M3 = M1 @ M2  // [[4, 4], [10, 8]]
  ```

#### 에러 처리
- ✨ **크기 검증**: 행렬 연산 시 자동으로 크기 확인
- ✨ **ValueError 예외**: 크기 불일치 시 명확한 에러 메시지
  ```
  ValueError: matrix dimension mismatch: (2x2) + (2x3)
  ```

#### 복합 연산 지원
- ✨ **연산 체이닝**: 여러 연산을 조합 가능
  ```finelang
  let result = (A + B) * 2 - C @ D
  ```

### 구현 세부사항
- **interpreter_eval_binary()**: 행렬 연산 케이스 추가
- **행렬 덧셈/뺄셈**: O(m×n) 요소별 연산
- **스칼라 곱**: 교환법칙 자동 처리
- **행렬 곱**: O(m×n×k) 표준 알고리즘

### 파일 변경
- **수정**: `src/interpreter.c` - 행렬 연산 로직 추가 (+150 LOC)
- **신규**: `test_add.fine` - 간단한 덧셈 테스트
- **신규**: `examples/matrix_operations.fine` - 모든 연산 종합 테스트
- **신규**: `V2.2_COMPLETION_REPORT.md` - 행렬 연산 구현 문서

### 성과
- 🎯 **완전한 선형대수 연산 지원**
- 🎯 **NumPy 스타일 @ 연산자**
- 🎯 **AI/ML 알고리즘 구현 가능**
- 🎯 **수학적 정확성 보장**

## v2.1.0 (2025-01-09) - 행렬 타입 및 기본 기능 📊

### 새로운 기능

#### 행렬 타입 추가
- ✨ **VAL_MATRIX 타입**: 2차원 행렬을 위한 새로운 값 타입
  ```finelang
  let m = [[1, 2, 3], [4, 5, 6]]  // 2x3 행렬
  ```

- ✨ **자동 행렬 감지**: 중첩 배열을 행렬로 자동 변환
  - 모든 행의 길이가 같으면 행렬로 변환
  - 그렇지 않으면 일반 배열로 유지

- ✨ **행렬 출력**: 가독성 높은 행렬 표시
  ```
  Matrix(2x3)[
    [1, 2, 3],
    [4, 5, 6]
  ]
  ```

- ✨ **행렬 인덱싱**: 행 단위 접근 지원
  ```finelang
  let m = [[1, 2], [3, 4]]
  print(m[0])  // [1, 2]
  print(m[1])  // [3, 4]
  ```

#### 내부 구조
- ✨ **AST_MATRIX 노드**: 파서에서 행렬 전용 AST 노드
- ✨ **value_create_matrix()**: 행렬 생성 함수
- ✨ **행렬 메모리 관리**: copy, free 함수에 행렬 지원 추가

### 구현 세부사항
- **Parser**: 중첩 배열 감지 로직 추가 (parser_parse_array)
- **Interpreter**: AST_MATRIX 케이스 처리, 행렬 인덱싱 지원
- **Value 시스템**: 행렬 복사 및 메모리 해제 지원

### 파일 변경
- **수정**: `src/interpreter.h` - VAL_MATRIX 타입, matrix 구조체 추가
- **수정**: `src/interpreter.c` - AST_MATRIX 처리, 인덱싱, copy/free 지원
- **수정**: `src/parser.h` - AST_MATRIX 노드 타입, matrix 데이터 구조
- **수정**: `src/parser.c` - 중첩 배열 감지 및 행렬 변환 로직
- **신규**: `test_matrix.fine` - 간단한 행렬 테스트
- **신규**: `examples/matrix_basic.fine` - 행렬 기본 기능 예제
- **신규**: `MATRIX_IMPLEMENTATION.md` - 행렬 구현 문서

### 다음 단계 (v2.2.0 계획)
- [ ] 행렬 연산: +, -, *, @ (행렬 곱)
- [ ] 행렬 유틸리티: shape(), transpose(), dot()
- [ ] 2차원 인덱싱: m[i][j] 형태 지원

### 성과
- 🎯 **AI/ML을 위한 행렬 타입 도입**
- 🎯 **직관적인 행렬 생성 문법**
- 🎯 **메모리 효율적인 2D 배열 구조**

## v2.0.0 (2025-01-27) - 모듈 시스템 완성 🎉

### 새로운 기능

#### 완전한 모듈 네임스페이스 지원
- ✨ **직접 메서드 호출**: `module.function(args)` 형태로 직접 호출 가능
  ```finelang
  import math
  print(math.abs(-5))      # 5
  print(math.max(10, 20))  # 20
  print(math.pow(2, 8))    # 256
  ```

- ✨ **단항 마이너스 파싱**: `-x`, `-5`, `-(-10)` 모두 지원
  ```finelang
  let x = -5
  let y = -x        # 5
  let z = -(-10)    # 10
  ```

- ✨ **중첩 함수 호출**: 모듈 함수를 중첩하여 호출 가능
  ```finelang
  let result = math.abs(math.min(-5, 3))  # 5
  ```

#### 파서 개선
- ✨ **parser_parse_unary()** 함수 추가: 단항 연산자 전용 파싱 계층
- ✨ **재귀적 단항 연산자 처리**: `---5` 같은 다중 단항 연산자 지원

#### 인터프리터 개선
- ✨ **AST_METHOD_CALL에 VAL_MODULE 처리 추가**: 모듈 함수를 메서드처럼 호출
- ✨ **모듈 함수 호출 환경 관리**: 클로저와 함께 올바른 환경에서 실행

### 버그 수정
- 🐛 **stdlib/math.fine**: 단항 마이너스를 사용하도록 abs 함수 복원

### 파일 변경
- **수정**: `src/parser.h`, `src/parser.c` - parser_parse_unary 함수 추가
- **수정**: `src/interpreter.c` - AST_METHOD_CALL에 VAL_MODULE 처리 추가, AST_UNARY_OP 개선
- **수정**: `stdlib/math.fine` - abs 함수 복원
- **신규**: `examples/modules_complete.fine` - 모든 모듈 기능 종합 데모

### 성과
- 🎯 **모듈 시스템 v1.9.0 → v2.0.0 달성**
- 🎯 **Python 스타일 모듈 import 완성**
- 🎯 **직관적인 모듈 사용 경험 제공**

## v1.9.1 (2025-01-27) - 모듈 네임스페이스 개선 🔧

### 새로운 기능

#### 모듈 네임스페이스 접근
- ✨ **VAL_MODULE 타입**: 모듈을 값 객체로 표현
  ```finelang
  import math
  print(math)  # <module 'math'>
  ```

- ✨ **DOT_ACCESS로 모듈 exports 접근**: `module.symbol` 형태로 export된 값 접근
  ```finelang
  import math
  let abs_func = math.abs  # 함수 가져오기
  let pi = math.PI         # 상수 가져오기
  ```

- ✨ **함수를 변수에 저장 후 호출**:
  ```finelang
  import math
  let abs_func = math.abs
  print(abs_func(0 - 10))  # 10
  ```

#### 버그 수정
- 🐛 **AST_UNARY_OP 케이스 추가**: 단항 연산자 처리 로직 구현 (파서는 아직 미지원)
- 🐛 **value_print에 VAL_MODULE, VAL_FUNCTION 추가**: 모듈과 함수 객체 출력 개선

### 알려진 제한사항
- ⚠️ **직접 메서드 호출 불가**: `module.function(args)` 형태 직접 호출 안 됨 (파서 수정 필요)
- ⚠️ **단항 마이너스 파싱 미지원**: `-5` 같은 리터럴 파싱 안 됨 (`0 - 5` 사용 필요)
- ⚠️ **from...import 미지원**: 특정 심볼만 가져오기 불가 (v2.0 예정)
- ⚠️ **as 별칭 미완성**: 파싱은 되지만 동작 안 함 (v2.0 예정)

### 파일 변경
- **수정**: `src/interpreter.h` - VAL_MODULE 타입 및 module 필드 추가
- **수정**: `src/interpreter.c` - value_create_module, AST_UNARY_OP 케이스, DOT_ACCESS에 모듈 처리 추가
- **수정**: `stdlib/math.fine` - abs 함수에서 단항 마이너스 대신 이항 빼기 사용
- **신규**: `examples/modules_v2_demo.fine` - 모듈 네임스페이스 기능 데모

## v1.9.0 (2025-01-27) - 모듈 시스템 📦

### 새로운 기능

#### 모듈 시스템 (Module System)
- ✨ **import 문**: 다른 파일의 모듈을 가져오기
  ```finelang
  import math
  import string
  ```

- ✨ **export 문**: 함수/변수를 모듈 외부로 내보내기
  ```finelang
  export fn abs(x) {
      if x < 0 { return -x }
      return x
  }
  
  export let PI = 3.14159265359
  ```

- ✨ **모듈 캐싱**: 동일한 모듈을 여러 번 import해도 한 번만 로드
- ✨ **표준 라이브러리**: stdlib 디렉토리에서 자동으로 모듈 검색
  - `stdlib/math.fine`: abs, max, min, pow, factorial, PI, E
  - `stdlib/string.fine`: length, repeat, is_empty

#### 새로운 토큰
- `TOKEN_IMPORT`: import 키워드
- `TOKEN_EXPORT`: export 키워드  
- `TOKEN_FROM`: from 키워드 (향후 사용)

#### 새로운 AST 노드
- `AST_IMPORT`: import 문을 위한 AST 노드
- `AST_EXPORT`: export 문을 위한 AST 노드

### 내부 개선
- ✨ **Module 구조체**: 모듈 정보를 저장 (이름, 경로, AST, exports 환경)
- ✨ **ModuleCache**: 로드된 모듈을 캐시하여 중복 로딩 방지
- ✨ **module_resolve_path()**: stdlib과 현재 디렉토리에서 모듈 파일 검색
- ✨ **module_load()**: 모듈 파일을 읽고 파싱하여 Module 객체 생성

### 알려진 제한사항
- ⚠️ **네임스페이스 접근 미지원**: `module.function()` 형태의 호출 불가 (v2.0 예정)
- ⚠️ **from...import 미지원**: 특정 심볼만 가져오기 불가 (v2.0 예정)
- ⚠️ **as 별칭 미완성**: 파싱은 되지만 동작 안 함 (v2.0 예정)

### 파일 변경
- **신규**: `src/module.h`, `src/module.c` - 모듈 관리 시스템
- **신규**: `stdlib/math.fine` - 수학 함수 표준 라이브러리
- **신규**: `stdlib/string.fine` - 문자열 함수 표준 라이브러리
- **신규**: `examples/modules_basic.fine` - 모듈 시스템 예제
- **수정**: `src/lexer.h`, `src/lexer.c` - import/export/from 토큰 추가
- **수정**: `src/parser.h`, `src/parser.c` - import/export 파싱 추가
- **수정**: `src/interpreter.h`, `src/interpreter.c` - 모듈 실행 로직 추가
- **수정**: `Makefile` - module.c 컴파일 추가

## v1.8.0 (2025-01-26) - 고차 함수 🎯

### 새로운 기능

#### 고차 함수 (Higher-Order Functions)
- ✨ **map(function, array)**: 배열의 각 요소에 함수를 적용
  ```finelang
  fn double(x) { return x * 2 }
  let result = map(double, [1, 2, 3])  # [2, 4, 6]
  ```

- ✨ **filter(predicate, array)**: 조건을 만족하는 요소만 필터링
  ```finelang
  fn is_positive(x) { return x > 0 }
  let result = filter(is_positive, [-1, 2, -3, 4])  # [2, 4]
  ```

- ✨ **reduce(function, array, initial)**: 배열을 단일 값으로 축약
  ```finelang
  fn add(acc, x) { return acc + x }
  let result = reduce(add, [1, 2, 3, 4], 0)  # 10
  ```

#### 일급 함수 (First-Class Functions)
- ✨ **함수를 값으로 전달 가능**
  - 함수를 변수에 저장
  - 함수를 다른 함수의 인자로 전달
  - `value_copy`에 `VAL_FUNCTION` case 추가

#### 함수 체이닝
- ✨ **여러 고차 함수 조합 가능**
  ```finelang
  let result = reduce(add, filter(is_positive, map(square, numbers)), 0)
  ```

### 내부 개선
- 🔧 함수 값 복사 메커니즘 개선 (클로저 참조 공유)
- 🔧 map, filter, reduce에서 환경 생성 및 정리 최적화

### 문서
- 📚 `examples/higher_order_functions.fine` 예제 추가
- 📚 고차 함수 사용법 및 체이닝 예제 포함

---

## v1.7.0 (2025-01-26) - 스택 추적 🔍

### 새로운 기능

#### 스택 추적 (Stack Trace)
- ✨ **예외 발생 시 함수 호출 스택 표시**
  - Python 스타일의 `Traceback (most recent call last):` 포맷
  - 각 프레임에 파일 이름, 라인 번호, 함수 이름 표시
  - 예외가 발생한 정확한 위치 추적 가능

- ✨ **라인 번호와 파일 이름 정보**
  - 모든 AST 노드에 라인 번호 추적
  - 각 스택 프레임에 파일 이름 저장
  - 디버깅 시 정확한 위치 파악 용이

- ✨ **스택 깊이 제한**
  - 최대 스택 깊이: 1000 프레임
  - 초과 시 `RecursionError` 자동 발생
  - 무한 재귀로부터 보호

#### 지원되는 예외 타입
모든 예외에 스택 추적 자동 첨부:
- `ZeroDivisionError` - 0으로 나누기
- `IndexError` - 배열 인덱스 범위 초과
- `KeyError` - 딕셔너리 키 없음
- `AssertionError` - assert 실패
- `RuntimeError` - throw 문
- `RecursionError` - 재귀 깊이 초과

### 코드 예제

```finelang
# 스택 추적 예제
fn c() {
    let x = 1 / 0  # ZeroDivisionError 발생
}

fn b() {
    c()
}

fn a() {
    b()
}

a()

# 출력:
# Traceback (most recent call last):
#   File "<input>", line 2, in c
#   File "<input>", line 6, in b
#   File "<input>", line 10, in a
# ZeroDivisionError: division by zero
```

```finelang
# 재귀 깊이 제한
fn recursive(n) {
    if n > 0 {
        recursive(n + 1)
    }
}

recursive(1)

# 출력:
# Traceback (most recent call last):
#   File "<input>", line 3, in recursive
#   File "<input>", line 3, in recursive
#   ... (1000 frames)
# RecursionError: maximum recursion depth exceeded
```

### 새로운 예제
- 📚 `examples/stack_trace_test.fine` - 스택 추적 기본 예제
- 📚 `examples/recursion_limit_test.fine` - 재귀 깊이 제한 테스트
- 📚 `examples/exception_types_test.fine` - 다양한 예외 타입 테스트

### 구현 세부사항
- 스택 프레임 연결 리스트로 구현
- 예외 복사 시 스택 추적도 깊은 복사
- 메모리 누수 방지를 위한 적절한 해제
- 함수 진입/종료 시 자동 스택 관리

---

## v1.6.0 (2025-01-26) - 고급 예외 처리 🎯

### 새로운 기능

#### 타입별 catch
- ✨ **특정 예외 타입만 잡기**: `catch ExceptionType as e`
  - `catch ZeroDivisionError as e` - 0으로 나누기 예외만 잡기
  - `catch IndexError as e` - 인덱스 오류만 잡기
  - `catch KeyError as e` - 키 오류만 잡기
  - `catch AssertionError as e` - 단언 오류만 잡기
  - 타입이 맞지 않으면 예외 계속 전파

#### assert 문
- ✨ **조건 검증**: `assert condition, "message"`
  - 조건이 false이면 AssertionError 발생
  - 커스텀 에러 메시지 지정 가능
  - 함수 내에서 전제 조건 검증에 유용
  - 예: `assert x > 0, "x must be positive"`

#### 새로운 예외 타입
- ✨ **AssertionError**: assert 문 실패 시 발생
  - `assert 0, "This fails"` → "AssertionError: This fails"

### 코드 예제

```finelang
# 타입별 catch
try {
    let x = 10 / 0
} catch ZeroDivisionError as e {
    print("Caught:", e)
}

# assert 문
fn validate_positive(num) {
    assert num > 0, "Number must be positive"
    return num
}

try {
    validate_positive(-5)
} catch AssertionError as e {
    print("Validation failed:", e)
}
```

### 새로운 예제
- 📚 `examples/v160_advanced_exceptions.fine` (160 lines)
  - 타입별 catch (ZeroDivisionError, IndexError, KeyError)
  - 타입 불일치 시 예외 전파
  - assert 문 성공/실패
  - 함수에서 assert 사용
  - 복합 예제

### 기술적 변경
- 🔧 Lexer: `TOKEN_AS`, `TOKEN_ASSERT` 토큰 추가
- 🔧 Parser: `exception_type` 필드를 `try_catch` 구조체에 추가
- 🔧 Parser: `AST_ASSERT` 노드 추가
- 🔧 Parser: `parser_parse_try_catch()` - 타입별 catch 파싱 지원
- 🔧 Parser: `parser_parse_assert()` 함수 구현
- 🔧 Interpreter: AST_TRY_CATCH에서 예외 타입 매칭 로직 추가
- 🔧 Interpreter: AST_ASSERT 케이스 구현
- 🔧 Interpreter: AssertionError 생성 및 처리

### 개선 사항
- 🎯 더 정밀한 예외 처리 가능
- 🎯 조건 검증이 간결해짐
- 🎯 디버깅이 더 쉬워짐

---

## v1.5.0 (2025-01-26) - 예외 처리 시스템 🛡️

### 새로운 기능

#### 예외 처리
- ✨ **try-catch-finally**: 예외를 잡고 처리하는 구조
  - `try { }` - 예외가 발생할 수 있는 코드 블록
  - `catch e { }` - 예외를 잡아 처리
  - `finally { }` - 항상 실행되는 정리 코드
  
- ✨ **throw 문**: 예외 발생
  - `throw "error message"` - 문자열을 RuntimeError로 래핑
  - 함수에서 예외 발생 가능
  
#### 내장 예외 타입
- ✨ **ZeroDivisionError**: 0으로 나누기 시도
  - `10 / 0` → "ZeroDivisionError: division by zero"
  
- ✨ **IndexError**: 배열 인덱스 범위 초과
  - `arr[100]` → "IndexError: list index out of range: 100"
  
- ✨ **KeyError**: 딕셔너리 키 없음
  - `dict["nonexistent"]` → "KeyError: key not found: 'nonexistent'"
  
- ✨ **RuntimeError**: 일반 런타임 오류
  - `throw "message"` → "RuntimeError: message"

### 새로운 예제
- 📚 `examples/v150_exceptions.fine` (180 lines)
  - 기본 try-catch
  - throw 문
  - 내장 예외 타입 (ZeroDivisionError, IndexError, KeyError)
  - finally 블록
  - 중첩 try-catch
  - 클래스 메서드에서 예외
  - 예외 전파
  - 반복문에서 예외 처리
  
- 📚 `examples/test_exceptions_simple.fine` - 간단한 예외 테스트

### 기술적 변경
- 🔧 Lexer: `TOKEN_TRY`, `TOKEN_CATCH`, `TOKEN_THROW`, `TOKEN_FINALLY` 토큰 추가
- 🔧 Parser: `AST_TRY_CATCH`, `AST_THROW` 노드 추가
- 🔧 Parser: `parser_parse_try_catch()`, `parser_parse_throw()` 함수 구현
- 🔧 Interpreter: `VALUE_EXCEPTION` 타입 추가
- 🔧 Interpreter: `value_create_exception()` 함수 추가
- 🔧 Interpreter: 예외 전파 메커니즘 구현
- 🔧 Interpreter: `current_exception`, `has_exception` 필드 추가
- 🔧 Interpreter: 기존 연산에 예외 추가 (/, 배열 인덱싱, 딕셔너리 접근)

### 개선 사항
- 📖 `EXCEPTIONS.md` 예외 처리 가이드 추가
- 📖 `README.md` 예외 처리 예제 추가
- 📖 `docs/EXCEPTION_DESIGN.md` 설계 문서 추가

### 예외 전파 동작
- 함수에서 예외 발생 → 상위 호출자로 자동 전파
- try 블록에서 잡히지 않으면 → 계속 상위로 전파
- finally 블록 → 예외 여부와 관계없이 항상 실행

---

## v1.4.0 (2025-01-26) - 상속과 다형성 🏛️

### 새로운 기능

#### 클래스 상속
- ✨ **extends 키워드**: 클래스 상속 지원
  - `class Child extends Parent` 문법
  - 부모 필드 자동 상속
  - 부모 메서드 자동 상속
  - 다단계 상속 지원 (3단계 이상)

#### 메서드 오버라이딩
- ✨ **메서드 오버라이딩**: 자식 클래스에서 부모 메서드 재정의
  - 이름이 같으면 자식 메서드가 부모 메서드 대체
  - 다형성(Polymorphism) 지원

#### super 키워드
- ✨ **super.method()**: 부모 클래스 메서드 명시적 호출
  - `super.constructor(args)` - 부모 생성자 호출
  - `super.method()` - 부모 메서드 호출
  - `this` 컨텍스트 유지

### 새로운 예제
- 📚 `examples/v140_features.fine` (225 lines)
  - Animal → Dog 기본 상속
  - Shape → Rectangle 메서드 오버라이딩
  - Person → Student super 키워드
  - Vehicle → Car → ElectricCar 다단계 상속
  - Model → NeuralNetwork → CNN AI 모델 상속

### 기술적 변경
- 🔧 Lexer: `TOKEN_EXTENDS`, `TOKEN_SUPER` 토큰 추가
- 🔧 Parser: `AST_SUPER` 노드 추가
- 🔧 Parser: `class_def`에 `parent_class` 필드 추가
- 🔧 Parser: `parser_parse_class()` - extends 절 파싱
- 🔧 Interpreter: 상속 체인 병합 로직 구현
- 🔧 Interpreter: 메서드 오버라이드 감지 시스템
- 🔧 Interpreter: `AST_SUPER` 핸들러 구현

### 개선 사항
- 📖 `CLASSES.md` 업데이트 (상속, super 가이드 추가)
- 📖 `README.md` 업데이트 (OOP 기능 강조)
- 📖 `RELEASE_v130_v140.md` 릴리스 노트 추가
- 📖 `IMPLEMENTATION_SUMMARY_v130_v140.md` 구현 요약

---

## v1.3.0 (2025-01-26) - 클래스 완성 ✨

### 새로운 기능

#### 필드 접근과 수정
- ✨ **필드 읽기**: `obj.field` 문법으로 필드 값 읽기
- ✨ **필드 수정**: `obj.field = value` 문법으로 필드 값 변경
  - `AST_FIELD_ASSIGN` 노드 추가

#### this 키워드
- ✨ **this 완전 구현**: 메서드 내에서 현재 인스턴스 참조
  - 메서드 호출 시 자동으로 `this` 바인딩
  - `this.field` 문법으로 필드 접근
  - `this.method()` 문법으로 메서드 호출

#### 생성자 메서드
- ✨ **constructor 메서드**: 인스턴스 생성 시 자동 실행
  - `new` 키워드 사용 시 자동 호출
  - `this`로 필드 초기화 가능

### 새로운 예제
- 📚 `examples/v130_features.fine` (138 lines)
  - Person 클래스 (constructor 데모)
  - Counter 클래스 (this, increment 데모)
  - Point 클래스 (필드 수정 데모)

### 기술적 변경
- 🔧 Parser: `AST_FIELD_ASSIGN` 노드 추가
- 🔧 Parser: `parser_parse_expression()` - 필드 대입 파싱
- 🔧 Interpreter: `AST_DOT_ACCESS` - 필드 읽기 지원
- 🔧 Interpreter: `AST_FIELD_ASSIGN` - 필드 쓰기 구현
- 🔧 Interpreter: `AST_METHOD_CALL` - this 자동 바인딩
- 🔧 Interpreter: `AST_NEW` - constructor 자동 호출
- 🔧 Interpreter: `AST_THIS` - 현재 인스턴스 반환

### 개선 사항
- 📖 `CLASSES.md` 업데이트 (필드, this, constructor 가이드)

---

## v1.2.0 (2025-01-09)

### 새로운 기능

#### 클래스 (Class) 지원 🎯
- ✨ **객체 지향 프로그래밍** 지원
  - `class` 키워드로 클래스 정의
  - `new` 키워드로 인스턴스 생성
  - 필드(fields)와 메서드(methods) 정의
  - `.` 연산자로 메서드 호출
  - `->` 연산자로 메서드 호출 (선택적)

#### 새로운 토큰
- 🔧 `TOKEN_CLASS`: 클래스 정의
- 🔧 `TOKEN_THIS`: 현재 인스턴스 참조 (예약어)
- 🔧 `TOKEN_ARROW`: 메서드 호출 연산자 (`->`)

#### 새로운 예제
- 📚 `examples/classes.fine`: 기본 클래스 사용법
- 📚 `examples/ai_classes.fine`: AI 모델 클래스 활용
- 📚 `examples/methods.fine`: 메서드 호출 예제

### 개선 사항
- 📖 클래스 전용 가이드 문서 추가 (`CLASSES.md`)
- 📖 README에 클래스 사용 예제 추가
- 🎨 AI/ML 워크플로우를 위한 클래스 디자인 패턴 제시

### 기술적 변경
- 🔨 Lexer: `TOKEN_CLASS`, `TOKEN_THIS`, `TOKEN_ARROW` 토큰 추가
- 🔨 Parser: `AST_CLASS`, `AST_NEW`, `AST_THIS`, `AST_METHOD_CALL` 노드 추가
- 🔨 Parser: `parser_parse_class()` 함수 구현
- 🔨 Interpreter: `VAL_CLASS`, `VAL_INSTANCE` 타입 추가
- 🔨 클래스 정의, 인스턴스 생성, 메서드 호출 실행 엔진 구현

### 알려진 제약
- ⚠️ 필드 직접 접근 미지원 (`obj.field`)
- ⚠️ 필드 수정 미지원 (`obj.field = value`)
- ⚠️ `this` 키워드 미완성
- ⚠️ 상속 미지원
- ⚠️ 생성자 메서드 미지원

---

## v1.1.0 (2025-01-26)

### 새로운 기능

#### 자료 구조 지원
- ✨ **딕셔너리(해시맵)** 타입 추가
  - `{key: value}` 문법으로 딕셔너리 생성
  - `dict["key"]` 문법으로 값 접근
  - 중첩 자료 구조 완벽 지원 (배열 안의 딕셔너리, 딕셔너리 안의 배열)

#### 내장 함수 확장
- 🔧 `keys(dict)`: 딕셔너리의 모든 키를 배열로 반환
- 🔧 `values(dict)`: 딕셔너리의 모든 값을 배열로 반환
- 🔧 `len(dict)`: 딕셔너리 크기 반환 지원 추가

#### 새로운 예제
- 📚 `examples/dictionary.fine`: 딕셔너리 기본 사용법
- 📚 `examples/nested_structures.fine`: 중첩 자료 구조 활용
- 📚 `examples/ai_data_structures.fine`: AI/ML 데이터 처리 예제

### 개선 사항
- 📖 자료 구조 전용 가이드 문서 추가 (`DATA_STRUCTURES.md`)
- 📖 README에 딕셔너리 사용 예제 추가
- 🎨 AI/ML 워크플로우에 적합한 데이터 구조 패턴 제시

### 기술적 변경
- 🔨 Lexer: `TOKEN_COLON`, `TOKEN_STRUCT`, `TOKEN_NEW` 토큰 추가
- 🔨 Parser: `AST_DICT` 노드 타입과 `parser_parse_dict()` 함수 추가
- 🔨 Interpreter: `VAL_DICT` 타입과 딕셔너리 연산 구현
- 🔨 해시맵 기반 딕셔너리 구현 (선형 탐색, 향후 최적화 예정)

---

## v1.0.0 (2025-01-26)

### 초기 릴리스

#### 핵심 기능
- ⚡ C로 작성된 고성능 인터프리터
- 🧮 벡터 연산 내장 지원
  - 벡터 덧셈/뺄셈/곱셈 (`+`, `-`, `*`)
  - 내적 연산자 (`@`)
- 🔧 기본 프로그래밍 기능
  - 변수 선언 (`let`)
  - 함수 정의 (`fn`)
  - 조건문 (`if`/`else`)
  - 반복문 (`for`, `while`)
  - 재귀 함수
- 📦 배열 자료 구조

#### 내장 함수
- `print()`: 값 출력
- `range()`: 범위 생성
- `len()`: 배열 길이
- `sum()`: 배열 합계

#### 실행 모드
- 🚀 REPL (대화형) 모드
- 📄 파일 실행 모드

#### 예제 프로그램
- `hello.fine`: Hello World
- `fibonacci.fine`: 피보나치 수열
- `factorial.fine`: 팩토리얼
- `vector_ops.fine`: 벡터 연산
- `linear_regression.fine`: 선형 회귀
- `benchmark.fine`: 성능 측정

#### 문서
- 📖 README: 프로젝트 소개 및 전체 개요
- 📖 SYNTAX_GUIDE: 완전한 문법 가이드
- 📖 CHANGELOG: 버전별 변경 이력

#### 성능
- ⚡ 벤치마크: 16ms (복잡한 벡터/재귀 연산)
- 🎯 C 수준의 실행 속도

---

## 향후 계획

### v1.2.0 (예정)
- [x] 클래스 (Class) 지원
- [x] 객체 지향 프로그래밍
- [x] 메서드 호출
- [ ] 필드 접근/수정
- [ ] this 키워드 완성

### v1.3.0 (예정)
- [ ] 딕셔너리 값 수정 기능
- [ ] 딕셔너리 키 삭제 기능
- [ ] 해시 테이블 최적화 (O(1) 조회)
- [ ] 구조체(struct) 타입 추가
- [ ] 파일 I/O 기능

### v1.3.0 (예정)
- [ ] 모듈 시스템
- [ ] 표준 라이브러리 확장
- [ ] JIT 컴파일 최적화
- [ ] 병렬 처리 지원

### v2.0.0 (장기 계획)
- [ ] 타입 시스템 (선택적 타입 어노테이션)
- [ ] GPU 가속 벡터 연산
- [ ] NumPy/TensorFlow 연동
- [ ] 패키지 관리자
- [ ] LSP (Language Server Protocol) 지원

---

## 기여

버그 리포트, 기능 제안, 풀 리퀘스트를 환영합니다!

## 라이선스

MIT License
