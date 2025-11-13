# FineLang 변경 이력

## v2.3.2 (2025-11-13) - 배열 동적 확장 🚀

### 핵심 기능

#### 배열 동적 확장 (Dynamic Array Growth)
- **기능**: 배열 인덱스 할당 시 범위를 벗어나면 자동으로 배열 크기 증가
- **이전**: `arr[index] = value`에서 index가 범위 밖이면 IndexError 발생
- **현재**: 자동으로 배열 확장, 빈 공간은 `null`로 초기화

**사용 예제**:
```finelang
let arr = []
print(arr)        # []

arr[0] = 10
print(arr)        # [10]

arr[5] = 100
print(arr)        # [10, null, null, null, null, 100]

arr[2] = 30
print(arr)        # [10, null, 30, null, null, 100]
```

**활용**:
- Stack, Queue 등 자료구조 구현 가능
- 동적 배열이 필요한 모든 알고리즘에 사용 가능
- Python/JavaScript 스타일의 배열 사용 가능

**구현 세부사항**:
```c
// interpreter.c: AST_INDEX_ASSIGN 처리
if (idx >= array->data.array.count) {
    int new_count = idx + 1;
    array->data.array.elements = realloc(array->data.array.elements, 
                                          sizeof(Value*) * new_count);
    
    // 새로 추가된 공간을 null로 초기화
    for (int i = array->data.array.count; i < new_count; i++) {
        array->data.array.elements[i] = value_create_null();
    }
    
    array->data.array.count = new_count;
}
```

### stdlib 추가

#### stdlib/data_structures.fine - 자료구조 라이브러리
배열 동적 확장을 활용한 4가지 자료구조 제공:

**1. Stack (스택) - LIFO**
```finelang
import data_structures as ds

let stack = []
stack = ds.stack_push(stack, 10)
stack = ds.stack_push(stack, 20)
let result = ds.stack_pop(stack)
stack = result["stack"]           # 새 스택으로 업데이트
print(result["value"])            # 20
print(ds.stack_peek(stack))       # 10
```

함수: `stack_push`, `stack_pop` (Dictionary 반환), `stack_peek`, `stack_size`, `stack_is_empty`

**2. Queue (큐) - FIFO**
```finelang
let queue = []
queue = ds.queue_enqueue(queue, 10)
queue = ds.queue_enqueue(queue, 20)
let result = ds.queue_dequeue(queue)
queue = result["queue"]           # 새 큐로 업데이트
print(result["value"])            # 10
print(ds.queue_peek(queue))       # 20
```

함수: `queue_enqueue`, `queue_dequeue` (Dictionary 반환), `queue_peek`, `queue_size`, `queue_is_empty`

**3. LinkedList (연결 리스트)**
```finelang
let list = ds.list_create()
list = ds.list_append(list, 10)
list = ds.list_prepend(list, 5)
print(ds.list_to_array(list))      # [5, 10]
let val = ds.list_get(list, 0)     # 5

# remove_at는 Dictionary 반환
let result = ds.list_remove_at(list, 0)
list = result["list"]
print(result["value"])             # 5
```

함수: `list_create`, `list_append`, `list_prepend`, `list_get`, `list_set`, 
      `list_remove_at` (Dictionary 반환), `list_find`, `list_to_array`, `list_size`, `list_is_empty`, `list_clear`

**4. Deque (양방향 큐)**
```finelang
let deque = ds.deque_create()
deque = ds.deque_push_back(deque, 10)
deque = ds.deque_push_front(deque, 5)
print(ds.deque_to_array(deque))    # [5, 10]

# pop 함수들은 Dictionary 반환
let result = ds.deque_pop_front(deque)
deque = result["deque"]
print(result["value"])             # 5

result = ds.deque_pop_back(deque)
deque = result["deque"]
print(result["value"])             # 10
```

함수: `deque_create`, `deque_push_front`, `deque_push_back`, 
      `deque_pop_front` (Dictionary 반환), `deque_pop_back` (Dictionary 반환),
      `deque_peek_front`, `deque_peek_back`, `deque_size`, `deque_is_empty`, `deque_to_array`, `deque_clear`

**참고**: FineLang은 pass-by-value이므로 함수 반환값을 재할당해야 합니다.

**삭제 연산 (pop, dequeue, remove_at)은 Dictionary 반환**:
```finelang
# ✅ 올바른 사용법
let result = ds.stack_pop(stack)
stack = result["stack"]        # 새 스택
let value = result["value"]    # 꺼낸 값

# 또는 한 줄로:
let result = ds.queue_dequeue(queue)
queue = result["queue"]

# List remove도 동일:
let result = ds.list_remove_at(list, index)
list = result["list"]
print(result["value"])
```

**추가 연산은 새 배열 반환**:
```finelang
stack = ds.stack_push(stack, value)  # ✅
queue = ds.queue_enqueue(queue, val) # ✅
list = ds.list_append(list, item)    # ✅
```

**성능 특성**:
- Stack: push/pop O(1)
- Queue: enqueue O(1), dequeue O(n)
- LinkedList: append O(1), prepend O(n), get/set O(1), remove O(n)
- Deque: push_back O(1), push_front O(n), pop_back O(1), pop_front O(n)

**구현 방식**:
- 모두 배열 기반으로 구현 (FineLang의 동적 배열 활용)
- LinkedList는 교육/참고용 배열 래퍼 (실제 포인터 기반 아님)
- 간단하고 이해하기 쉬운 구현에 초점

#### data_structures 모듈 (참고용)
- **Stack**: `stack_push`, `stack_pop`, `stack_peek`, `stack_size`, `stack_is_empty`
- **Queue**: `queue_enqueue`, `queue_dequeue`, `queue_peek`, `queue_size`, `queue_is_empty`
- **주의**: FineLang은 pass by value이므로 직접 배열 사용 권장

**직접 배열 사용 (권장)**:
```finelang
# Stack (LIFO)
let stack = []
stack[len(stack)] = 10    # push
stack[len(stack)] = 20
let top = stack[len(stack) - 1]  # peek/pop

# Queue (FIFO) - 단순 버전
let queue = []
queue[len(queue)] = "A"   # enqueue
let first = queue[0]      # peek
```

---

## v2.3.1 (2025-11-13) - 버그 수정 및 개선 🐛

### 중요 버그 수정

#### 1. 모듈 함수 호출 버그 수정 (Critical)
- **문제**: 모듈에서 export한 함수 호출 시 파라미터가 `null`로 전달되는 심각한 버그
  ```finelang
  # 이전에는 작동 안 함
  import math
  print(math.sum([10, 20, 30]))  # 0 반환 (버그!)
  ```
- **원인**: 함수 환경으로 전환한 후 인자를 평가해서 발생
- **수정**: 인자를 먼저 평가한 후 함수 환경으로 전환하도록 수정
- **영향**: 모든 모듈 함수가 정상 작동하게 됨

#### 2. 몫 연산자 `//` 버그 수정 (Critical)
- **문제**: `6 // 2` 같은 몫 연산이 주석으로 인식되어 Segmentation Fault 발생
- **원인**: Lexer에서 `//`를 주석으로 잘못 처리
- **수정**: `//`를 `TOKEN_FLOOR_DIV`로 정확히 토큰화 (주석은 `#`만 사용)
  ```finelang
  print(10 // 3)  # 3 (이제 정상 작동!)
  print(7 // 2)   # 3
  ```

#### 3. 배열 인덱스 할당 기능 추가 (Major)
- **문제**: `arr[i] = value` 구문이 완전히 작동하지 않음
- **해결**: `AST_INDEX_ASSIGN` 노드 타입 추가 및 구현
  ```finelang
  let arr = [10, 20, 30]
  arr[1] = 999
  print(arr)  # [10, 999, 30]
  
  # 배열 정렬 등 알고리즘 구현 가능
  let i = 0
  while i < len(arr) - 1 {
      if arr[i] > arr[i + 1] {
          let temp = arr[i]
          arr[i] = arr[i + 1]
          arr[i + 1] = temp
      }
      i = i + 1
  }
  ```

### 새로운 기능

#### 이스케이프 문자 지원
- **`\n`**: 개행 (줄바꿈)
- **`\t`**: 탭
- **`\r`**: 캐리지 리턴
- **`\\`**: 백슬래시
- **`\"`**: 큰따옴표
- **`\'`**: 작은따옴표
- **`\0`**: 널 문자

```finelang
print("줄1\n줄2\n줄3")
print("이름:\t홍길동")
print("경로: C:\\Users\\Documents")
print("그는 \"안녕\"이라고 말했다")
```

### API 개선

#### stdlib/math 모듈 - 배열 함수 API 간소화
이전에는 배열 길이를 명시적으로 전달해야 했으나, 이제 내부적으로 `len()` 호출:

**Before:**
```finelang
let arr = [10, 20, 30]
let n = len(arr)
print(math.sum(arr, n))
print(math.mean(arr, n))
```

**After:**
```finelang
let arr = [10, 20, 30]
print(math.sum(arr))      # 60
print(math.mean(arr))     # 20
```

**변경된 함수들:**
- `sum(arr)` - 합계
- `mean(arr)` - 평균
- `median(arr)` - 중앙값
- `variance(arr)` - 분산
- `std_dev(arr)` - 표준편차
- `array_max(arr)` - 최댓값
- `array_min(arr)` - 최솟값

### stdlib/math 모듈 완성 (50+ 함수)

#### 수학 상수
- `PI`, `E`, `PHI`, `SQRT2`, `SQRT3`, `LN2`, `LN10`, `LOG2E`, `LOG10E`

#### 기본 연산
- `abs(x)`, `sign(x)`, `max(a, b)`, `min(a, b)`, `clamp(x, min, max)`
- `square(x)`, `cube(x)`, `pow(x, y)`, `sqrt(x)`, `nroot(x, n)`

#### 반올림
- `floor(x)`, `ceil(x)`, `round(x)`, `trunc(x)`

#### 삼각함수
- `sin(x)`, `cos(x)`, `tan(x)`, `asin(x)`, `acos(x)`, `atan(x)`, `atan2(y, x)`
- `sinh(x)`, `cosh(x)`, `tanh(x)`

#### 지수 및 로그
- `exp_approx(x)`, `ln(x)`, `log10(x)`, `log2(x)`, `log(x, base)`

#### 조합론
- `factorial(n)`, `permutation(n, r)`, `combination(n, r)`
- `gcd(a, b)`, `lcm(a, b)`, `fibonacci(n)`

#### 배열 통계 (새로운 API)
- `sum(arr)`, `mean(arr)`, `median(arr)`
- `variance(arr)`, `std_dev(arr)`
- `array_max(arr)`, `array_min(arr)`

#### 기타
- `lerp(a, b, t)`, `normalize(x, min, max)`, `map_range(x, ...)`
- `is_even(n)`, `is_odd(n)`, `is_prime(n)`
- `radians(deg)`, `degrees(rad)`

### 테스트

모든 math 함수 테스트 통과:
```
✅ 수학 상수
✅ 기본 연산
✅ 거듭제곱
✅ 반올림
✅ 삼각함수
✅ 지수 및 로그
✅ 조합론
✅ 배열 통계
✅ 각도 변환
✅ 기타 함수
```

---

## v2.2.8 (2025-01-10) - Boolean 타입 ✅

### 새로운 기능

#### Boolean 리터럴 (Python 스타일)

- ✨ **`true` 키워드**: Boolean 참 값 (1로 평가)
  ```finelang
  let is_ready = true
  print(true)              # 1
  print(true == 1)         # 1
  ```

- ✨ **`false` 키워드**: Boolean 거짓 값 (0으로 평가)
  ```finelang
  let is_error = false
  print(false)             # 0
  print(false == 0)        # 1
  ```

- ✨ **조건문에서 직접 사용**
  ```finelang
  if true {
      print("항상 실행됨")
  }
  
  let is_enabled = true
  if is_enabled {
      print("활성화됨")
  }
  ```

- ✨ **함수 반환값으로 사용**
  ```finelang
  fn is_positive(x) {
      if x > 0 {
          return true
      } else {
          return false
      }
  }
  
  print(is_positive(5))    # 1 (true)
  print(is_positive(-3))   # 0 (false)
  ```

### 실용 예제

```finelang
# 설정 플래그
let debug_mode = false
let production_mode = true

if production_mode {
    print("운영 환경에서 실행 중")
}

# 검증 함수
fn is_valid_age(age) {
    if age >= 0 {
        if age <= 150 {
            return true
        }
    }
    return false
}

print(is_valid_age(25))    # 1 (true)
print(is_valid_age(-5))    # 0 (false)
print(is_valid_age(200))   # 0 (false)

# 상태 체크
fn check_system() {
    let network_ok = true
    let disk_ok = true
    
    if network_ok {
        if disk_ok {
            return true
        }
    }
    return false
}
```

### 구현 세부사항

- **렉서 (Lexer)**:
  - `TOKEN_TRUE`, `TOKEN_FALSE` 토큰 타입 추가
  - 키워드 인식: `strcmp(value, "true")`, `strcmp(value, "false")`

- **파서 (Parser)**:
  - `TOKEN_TRUE` → `AST_NUMBER(1.0)` 변환
  - `TOKEN_FALSE` → `AST_NUMBER(0.0)` 변환
  - Primary expression 파싱에서 처리

- **의미론**:
  - `true`는 항상 숫자 1로 평가
  - `false`는 항상 숫자 0으로 평가
  - 기존 숫자 기반 조건문과 완벽 호환

### 테스트

- ✅ `test_boolean.fine`: Boolean 리터럴 기본 동작
- ✅ 비교 연산: `true == 1`, `false == 0`
- ✅ 조건문: `if true`, `if false`
- ✅ 함수 반환값
- ✅ 변수 할당

### 호환성

- Python 스타일 Boolean 리터럴
- 기존 숫자 기반 조건문과 100% 호환
- 하위 호환성 유지 (0/1 여전히 사용 가능)

---

## v2.2.7 (2025-01-10) - 문자열 연산 🔤

### 새로운 기능

#### 문자열 연산자 (Python 스타일)

- ✨ **문자열 연결 (`+`)**: 두 문자열을 연결
  ```finelang
  print("Hello" + " " + "World")  # "Hello World"
  let greeting = "Hi" + " there"
  print(greeting)  # "Hi there"
  ```

- ✨ **문자열 반복 (`*`)**: 문자열을 N번 반복
  ```finelang
  print("Ha" * 3)      # "HaHaHa"
  print(5 * "Ho")      # "HoHoHoHoHo"
  print("-" * 20)      # "--------------------"
  ```

- ✨ **문자열 비교 (`==`, `!=`)**: 문자열 동등성 비교
  ```finelang
  print("abc" == "abc")  # 1 (true)
  print("abc" != "def")  # 1 (true)
  ```

### 실용 예제

```finelang
# 테두리 만들기
let border = "=" * 30
print(border)
print("Title")
print(border)

# 동적 문자열 생성
let name = "FineLang"
let version = "2.2.7"
let message = name + " v" + version
print(message)  # "FineLang v2.2.7"

# 반복 패턴
let stars = "*" * 10
let spaces = " " * 5
print(stars + spaces + stars)
```

### 구현 세부사항

**이항 연산 확장** (`src/interpreter.c`)
- 문자열 + 문자열: `strcat()` 사용하여 연결
- 문자열 * 숫자: 반복 루프로 문자열 복제
- 숫자 * 문자열: 문자열 * 숫자와 동일
- 문자열 비교: `strcmp()` 사용

**호환성**
- 기존 숫자/배열/행렬 연산에 영향 없음
- 문자열 연산은 독립적으로 처리

---

## v2.2.6 (2025-01-10) - 배열/문자열 유틸리티 함수 🛠️

### 새로운 기능

#### 🎯 Python 스타일 메서드 체이닝 지원!

모든 유틸리티 함수는 **두 가지 방식**으로 사용 가능:
1. **함수 호출 방식**: `contains(arr, value)`
2. **메서드 체이닝 방식**: `arr.contains(value)` ⭐

```finelang
# 함수 방식
print(contains([1, 2, 3], 2))    # 1

# 메서드 방식 (더 직관적!)
print([1, 2, 3].contains(2))     # 1
```

#### 검색 함수
- ✨ **array.contains(value) / contains(array, value)**: 요소 포함 여부
  ```finelang
  print([1, 2, 3].contains(2))           # 1 (true)
  print("Hello".contains("ell"))          # 1 (true)
  ```

- ✨ **array.index_of(value) / index_of(array, value)**: 인덱스 찾기 (-1 반환)
  ```finelang
  print([1, 2, 3].index_of(2))           # 1
  print("Hello".index_of("l"))            # 2
  print([1, 2, 3].index_of(5))           # -1
  ```

#### 배열 조작 함수
- ✨ **array.append(value) / append(array, value)**: 요소 추가 (원본 유지)
  ```finelang
  let arr = [1, 2, 3]
  let new_arr = arr.append(4)
  print(arr)      # [1, 2, 3] (원본 유지)
  print(new_arr)  # [1, 2, 3, 4]
  ```

- ✨ **array.reverse() / reverse(array)**: 배열 뒤집기
  ```finelang
  print([1, 2, 3, 4, 5].reverse())  # [5, 4, 3, 2, 1]
  ```

#### 수학 함수
- ✨ **array.min() / min(array)**: 최솟값
  ```finelang
  print([5, 2, 8, 1, 9].min())  # 1
  ```

- ✨ **array.max() / max(array)**: 최댓값
  ```finelang
  print([5, 2, 8, 1, 9].max())  # 9
  ```

### 실용 예제

```finelang
# Python 스타일 메서드 체이닝!
let scores = [85, 92, 78, 95, 88]

if scores.contains(95) {
    print("Perfect score found!")
}

print("최고점:", scores.max())  # 95
print("최저점:", scores.min())  # 78

# 배열 조작 체이닝
let arr = [1, 2, 3]
let result = arr.append(4).append(5)
print(result)  # [1, 2, 3, 4, 5]

# 문자열 검색
let text = "Hello, World!"
if text.contains("World") {
    let pos = text.index_of("World")
    print("'World'는", pos, "번째에 있습니다")  # 7
}

# 인라인 메서드 호출
print([5, 2, 8, 1, 9].reverse().max())  # 9
```

### 구현 세부사항

**인터프리터 수정** (`src/interpreter.c`)
- **AST_METHOD_CALL 확장**: 배열/문자열 메서드 체이닝 지원
  - 배열 메서드: append, reverse, contains, index_of, min, max
  - 문자열 메서드: contains, index_of
- `append()`: 원본 유지, 새 배열 생성 (불변성)
- `contains()`: 배열 선형 검색, 문자열 strstr() 사용
- `index_of()`: 선형 검색, 문자열 포인터 연산
- `reverse()`: 새 배열 생성 및 역순 복사
- `min()`, `max()`: 선형 탐색

### 호환성
- ✅ 기존 코드와 100% 호환
- ✅ 배열과 문자열 모두 지원 (contains, index_of)
- ✅ null-safe (유효하지 않은 입력은 null 또는 -1 반환)

---

## v2.2.5 (2025-01-10) - 타입 체크 함수 🔍

### 새로운 기능

#### null 체크 함수
- ✨ **is_null(value)**: null 값인지 확인
  ```finelang
  let x = null
  if is_null(x) {
      print("x는 null입니다")
  }
  ```

#### 자료형 체크 함수
- ✨ **is_number(value)**: 숫자인지 확인
- ✨ **is_string(value)**: 문자열인지 확인
- ✨ **is_bool(value)**: boolean(0 또는 1)인지 확인
- ✨ **is_array(value)**: 배열인지 확인
- ✨ **is_dict(value)**: 딕셔너리인지 확인
- ✨ **is_matrix(value)**: 행렬인지 확인

  ```finelang
  let num = 42
  let str = "hello"
  let arr = [1, 2, 3]
  
  print(is_number(num))   # 1 (true)
  print(is_string(str))   # 1 (true)
  print(is_array(arr))    # 1 (true)
  ```

#### 자료형 확인 함수
- ✨ **typeof(value)**: 값의 자료형을 문자열로 반환
  ```finelang
  print(typeof(null))      # "null"
  print(typeof(42))        # "number"
  print(typeof("hi"))      # "string"
  print(typeof([1,2]))     # "array"
  print(typeof({x:10}))    # "dict"
  print(typeof([[1,2]]))   # "matrix"
  ```

### 실용 예제

#### 타입 안전 함수
```finelang
fn safe_divide(a, b) {
    if is_null(a) {
        print("Error: a is null")
        return null
    }
    if is_null(b) {
        print("Error: b is null")
        return null
    }
    if is_number(a) == 0 {
        print("Error: a is not a number")
        return null
    }
    if is_number(b) == 0 {
        print("Error: b is not a number")
        return null
    }
    if b == 0 {
        print("Error: division by zero")
        return null
    }
    return a / b
}
```

### 구현 세부사항

**인터프리터 수정** (`src/interpreter.c`)
- `is_null()`, `is_number()`, `is_string()`, `is_bool()` 함수 추가
- `is_array()`, `is_dict()`, `is_matrix()` 함수 추가
- `typeof()` 함수: 모든 자료형 문자열 반환

### 호환성
- ✅ 기존 코드와 100% 호환
- ✅ 모든 자료형 지원 (null, number, string, array, dict, matrix, function, class, instance, module, exception)

---

## v2.2.4 (2025-01-09) - 나머지/몫 연산자 ➗

### 새로운 기능

#### 나머지 연산자 (%)
- ✨ **나머지 계산**: 나눗셈의 나머지 반환
  ```finelang
  print(10 % 3)   # 1
  print(15 % 4)   # 3
  print(7 % 2)    # 1
  ```

- ✨ **홀수/짝수 판별**:
  ```finelang
  if num % 2 == 0 {
      print("짝수")
  } else {
      print("홀수")
  }
  ```

- ✨ **순환 인덱스**:
  ```finelang
  let circular_index = index % len(array)
  ```

#### 몫 연산자 (//)
- ✨ **내림 나눗셈**: 나눗셈의 몫(정수 부분) 반환
  ```finelang
  print(10 // 3)   # 3
  print(15 // 4)   # 3
  print(7 // 2)    # 3
  print(9.5 // 2)  # 4
  ```

- ✨ **시간 변환 활용**:
  ```finelang
  let total_minutes = 137
  let hours = total_minutes // 60    # 2
  let minutes = total_minutes % 60   # 17
  print(hours, "시간", minutes, "분")  # 2 시간 17 분
  ```

### 구현 세부사항

**렉서 수정** (`src/lexer.h`, `src/lexer.c`)
- `TOKEN_MODULO`: `%` 연산자 토큰
- `TOKEN_FLOOR_DIV`: `//` 연산자 토큰 (주석과 구분하기 위해 연속된 `/` 체크)

**파서 수정** (`src/parser.c`)
- `parser_parse_factor()`: `%`, `//` 연산자를 곱셈/나눗셈과 동일한 우선순위로 처리

**인터프리터 수정** (`src/interpreter.c`)
- `interpreter_eval_binary()`:
  - `%`: `fmod(l, r)` 사용 (부동소수점 나머지)
  - `//`: `floor(l / r)` 사용 (내림 나눗셈)
  - 0으로 나누기 예외 처리 추가

### 수학 함수 추가
- `#include <math.h>`: `fmod()`, `floor()` 함수 사용

### 실용 예제

**배수 체크**:
```finelang
if n % 3 == 0 {
    print(n, "은(는) 3의 배수")
}
```

**나머지와 몫의 관계**:
```finelang
let a = 17, b = 5
print((a // b) * b + (a % b))  # 17 (항상 a와 같음)
```

### 테스트 파일
- ✅ `test_modulo_floordiv.fine`: 기본 연산 테스트
- ✅ `examples/modulo_floordiv.fine`: 실용 예제

---

## v2.2.3 (2025-01-09) - 모듈 별칭(as) 완성 🏷️

### 새로운 기능

#### 모듈 별칭 지원
- ✨ **import module as alias**: 모듈을 별칭으로 사용
  ```finelang
  import math as m
  print(m.abs(-42))    # 42
  print(m.PI)          # 3.14159
  ```

- ✨ **긴 모듈 이름 단축**: 가독성과 타이핑 편의성 향상
  ```finelang
  import very_long_module_name as vlmn
  vlmn.some_function()
  ```

- ✨ **네이밍 충돌 방지**: 여러 모듈의 같은 이름 함수 구분
  ```finelang
  import utils as my_utils
  import other_lib as other
  ```

### 사용 예제

**기본 사용**:
```finelang
import math as m
print(m.max(10, 20))  # 20
print(m.min(5, 3))    # 3
```

**from...import와 혼용**:
```finelang
import math as m
from math import PI

print(m.abs(-5))     # 별칭으로 접근
print(PI)            # 직접 접근
```

**같은 모듈 여러 별칭** (테스트/디버깅용):
```finelang
import math as m1
import math as m2
print(m1.abs(-1))    # 1
print(m2.abs(-2))    # 2
```

### 구현 상태

- ✅ **파서**: `as` 키워드 파싱, alias 저장
- ✅ **인터프리터**: alias를 변수 이름으로 사용하여 모듈 등록
- ✅ **호환성**: 기존 import, from...import와 완벽 공존

### 문서 업데이트
- ✅ `SYNTAX_GUIDE.md`: 10.6 모듈 별칭 섹션 추가
- ✅ `examples/module_alias.fine`: 별칭 사용 예제

---

## v2.2.2 (2025-01-09) - from...import 구문 🎯

### 새로운 기능

#### from...import 구문
- ✨ **선택적 임포트**: 모듈에서 특정 심볼만 가져오기
  ```finelang
  from math import abs, max, PI
  print(abs(-5))      # 5 - math. 없이 직접 사용
  print(max(10, 20))  # 20
  print(PI)           # 3.14159
  ```

- ✨ **네임스페이스 오염 방지**: 필요한 함수만 현재 환경에 추가
  ```finelang
  # 방법 1: 전체 모듈 임포트 (네임스페이스 접근)
  import math
  print(math.abs(-5))
  
  # 방법 2: 특정 함수만 임포트 (직접 접근)
  from math import abs
  print(abs(-5))
  ```

- ✨ **여러 심볼 지원**: 쉼표로 구분하여 여러 함수/변수 가져오기
  ```finelang
  from string import length, repeat, is_empty
  ```

### 구현 세부사항

- **파서**: `TOKEN_FROM` 처리, `parser_parse_statement`에서 `TOKEN_FROM` 시작 지원
- **인터프리터**: `names` 배열을 순회하며 지정된 심볼만 현재 환경에 추가
- **호환성**: 기존 `import module` 및 `import module as alias` 구문과 공존

### 파일

- `src/parser.c`: `parser_parse_import` 함수 수정
- `src/interpreter.c`: `AST_IMPORT` 케이스에서 `names` 배열 처리
- `test_from_import.fine`: from...import 테스트 파일

---

# FineLang 변경 이력

## v2.2.2 (2025-01-09) - from...import 구문 🎯

### 새로운 기능

#### from...import 선택적 임포트
- ✨ **from module import name1, name2**: 모듈에서 특정 심볼만 가져오기
  ```finelang
  from math import abs, max, PI
  print(abs(-5))      # 5 - math. 접두사 없이 직접 사용
  print(max(10, 20))  # 20
  print(PI)           # 3.14159
  ```

- ✨ **네임스페이스 오염 방지**: 필요한 함수만 임포트하여 깔끔한 코드 작성
  ```finelang
  # 일반 import는 네임스페이스 접근
  import math
  print(math.abs(-5))  # math. 필요
  
  # from...import는 직접 접근
  from math import abs
  print(abs(-5))       # 직접 사용
  ```

- ✨ **여러 심볼 동시 임포트**: 쉼표로 구분하여 한 번에 여러 함수 가져오기
  ```finelang
  from string import length, repeat, is_empty
  ```

### 구현 세부사항

**파서 수정** (`src/parser.c`)
- `parser_parse_statement()`: `TOKEN_FROM`으로 시작하는 문장 처리 추가
- `parser_parse_import()`: 
  - `from module import ...` 구문 파싱
  - 심볼 이름들을 `names` 배열에 저장
  - `name_count`로 개수 추적

**인터프리터 연동** (`src/interpreter.c`)
- `AST_IMPORT` 처리 시 `names` 배열 확인
- 지정된 심볼만 현재 환경에 선택적으로 추가
- 모듈의 export에서 해당 심볼 검색 및 복사

### 호환성

**기존 import 구문과 완벽 공존**:
```finelang
# 모두 정상 작동
import math                    # 전체 모듈
import math as m               # 별칭
from math import abs, max      # 선택적 임포트
```

### 문서 업데이트
- ✅ `SYNTAX_GUIDE.md`: 10.5 from...import 구문 섹션 추가
- ✅ `README.md`: 모듈 시스템 기능 설명 업데이트

---

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
