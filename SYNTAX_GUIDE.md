# FineLang 문법 가이드 📖

> **FineLang v2.2.6** - AI/ML 작업에 최적화된 간결하고 강력한 프로그래밍 언어

**최종 업데이트**: 2025-01-10  
**버전**: v2.2.6

---

## 📑 목차

1. [기본 문법](#1-기본-문법)
2. [데이터 타입](#2-데이터-타입)
   - [Null](#21-null)
   - [Number](#22-number-숫자)
   - [String](#23-string-문자열)
   - [Array](#24-array-배열)
   - [Matrix](#25-matrix-행렬--v210)
   - [Dictionary](#26-dictionary-딕셔너리)
3. [연산자](#3-연산자)
4. [제어 구조](#4-제어-구조)
5. [함수](#5-함수)
6. [자료 구조](#6-자료-구조)
7. [객체 지향 프로그래밍](#7-객체-지향-프로그래밍)
8. [예외 처리](#8-예외-처리)
9. [내장 함수](#9-내장-함수)
   - [입출력 함수](#입출력)
   - [배열 함수](#배열-함수)
   - [딕셔너리 함수](#딕셔너리-함수)
   - [타입 체크 함수](#타입-체크-함수-v225)
   - [고차 함수](#고차-함수-v180)
10. [모듈 시스템](#10-모듈-시스템)

---

## 1. 기본 문법

### 1.1 주석

```finelang
# 한 줄 주석
let x = 42  # 인라인 주석도 가능
```

**특징**:
- `#`으로 시작
- 라인 끝까지 주석 처리
- 블록 주석은 미지원

### 1.2 변수 선언

```finelang
let x = 42               # 숫자
let name = "FineLang"    # 문자열
let pi = 3.14159         # 실수
let numbers = [1, 2, 3]  # 배열
let matrix = [[1, 2], [3, 4]]  # 행렬
```

**규칙**:
- `let` 키워드 사용
- 변수명: 영문자, 숫자, `_` 사용
- 변수명은 숫자로 시작 불가
- 동적 타입 (타입 선언 불필요)

---

## 2. 데이터 타입

### 2.1 Null
값이 없음을 나타내는 특수한 타입입니다.

```finelang
let x = null
let result = null

# null 체크
if is_null(x) {
    print("x는 null입니다")
}

# 함수에서 null 반환
fn maybe_divide(a, b) {
    if b == 0 {
        return null  # 유효하지 않은 경우 null 반환
    }
    return a / b
}

let value = maybe_divide(10, 0)
if is_null(value) {
    print("나눗셈 실패")
}
```

### 2.2 Number (숫자)
정수와 실수를 구분 없이 사용합니다.

```finelang
let integer = 42
let float = 3.14159
let negative = -10
let scientific = 1.5e10  # 과학적 표기법 (향후 지원 예정)
```

### 2.3 String (문자열)
큰따옴표(`"`)로 문자열을 생성합니다.

```finelang
let greeting = "Hello, World!"
let language = "FineLang"
let multiline = "첫 번째 줄\n두 번째 줄"  # 이스케이프 시퀀스

# 문자열 연산 (향후 지원 예정)
# let full = "Hello" + " " + "World"
```

### 2.4 Array (배열)
여러 값을 순서대로 저장하는 동적 배열입니다.

```finelang
let numbers = [1, 2, 3, 4, 5]
let names = ["Alice", "Bob", "Charlie"]
let mixed = [1, "two", 3.0, [4, 5]]  # 다양한 타입 가능
let nested = [[1, 2], [3, 4], [5, 6]]  # 중첩 배열

# 인덱싱 (0부터 시작)
let first = numbers[0]    # 1
let second = numbers[1]   # 2
let last = numbers[4]     # 5

# 수정
numbers[0] = 10
print(numbers)  # [10, 2, 3, 4, 5]
```

### 2.5 Matrix (행렬) 🆕 v2.1.0
2차원 배열이 자동으로 행렬로 인식됩니다.

```finelang
# 행렬 생성 (모든 행의 길이가 같아야 함)
let M = [[1, 2, 3],
         [4, 5, 6]]

# 2차원 인덱싱 (v2.2.1)
print(M[0][0])  # 1
print(M[1][2])  # 6

# 행렬 연산 (v2.2.0)
let A = [[1, 2], [3, 4]]
let B = [[5, 6], [7, 8]]

print(A + B)    # 덧셈
print(A - B)    # 뺄셈
print(2 * A)    # 스칼라 곱
print(A @ B)    # 행렬 곱
```

### 2.6 Dictionary (딕셔너리)
키-값 쌍으로 데이터를 저장하는 해시맵입니다. (v1.1.0+)

```finelang
# 딕셔너리 생성
let person = {name: "Alice", age: 30, city: "Seoul"}

# 값 접근
print(person["name"])   # Alice
print(person["age"])    # 30

# 빈 딕셔너리
let empty = {}
```

---

## 3. 연산자

### 산술 연산자

| 연산자 | 설명 | 예제 |
|--------|------|------|
| `+` | 덧셈 | `5 + 3` → 8 |
| `-` | 뺄셈 | `5 - 3` → 2 |
| `*` | 곱셈 | `5 * 3` → 15 |
| `/` | 나눗셈 | `6 / 3` → 2 |
| `%` | 나머지 | `10 % 3` → 1 |
| `//` | 몫 (내림 나눗셈) | `10 // 3` → 3 |

```finelang
let a = 10
let b = 3
print(a + b)   # 13
print(a - b)   # 7
print(a * b)   # 30
print(a / b)   # 3.333...
print(a % b)   # 1 (나머지)
print(a // b)  # 3 (몫)
```

**나머지와 몫 연산 활용**:
```finelang
# 홀수/짝수 판별
if num % 2 == 0 {
    print("짝수")
} else {
    print("홀수")
}

# 시간 변환 (분 -> 시:분)
let total_minutes = 137
let hours = total_minutes // 60    # 2
let minutes = total_minutes % 60   # 17
print(hours, "시간", minutes, "분")

# 순환 인덱스
let index = 15
let array_index = index % len(array)
```

### 비교 연산자

| 연산자 | 설명 | 예제 |
|--------|------|------|
| `==` | 같음 | `5 == 5` → 1 (true) |
| `!=` | 다름 | `5 != 3` → 1 (true) |
| `<` | 작음 | `3 < 5` → 1 (true) |
| `<=` | 작거나 같음 | `5 <= 5` → 1 (true) |
| `>` | 큼 | `5 > 3` → 1 (true) |
| `>=` | 크거나 같음 | `5 >= 5` → 1 (true) |

```finelang
let x = 10
let y = 20

if x < y {
    print("x is less than y")
}
```

### 벡터 연산자 (AI/ML 특화)

| 연산자 | 설명 | 예제 |
|--------|------|------|
| `+` | 요소별 덧셈 | `[1,2] + [3,4]` → [4,6] |
| `-` | 요소별 뺄셈 | `[5,6] - [1,2]` → [4,4] |
| `*` | 요소별 곱셈 | `[2,3] * [4,5]` → [8,15] |
| `@` | 내적 (dot product) | `[1,2,3] @ [4,5,6]` → 32 |

```finelang
let v1 = [1, 2, 3]
let v2 = [4, 5, 6]

let sum = v1 + v2       # [5, 7, 9]
let product = v1 * v2   # [4, 10, 18]
let dot = v1 @ v2       # 32 (1*4 + 2*5 + 3*6)
```

### 행렬 연산자 (v2.2.0+)

| 연산자 | 설명 | 예제 |
|--------|------|------|
| `+` | 행렬 덧셈 | `A + B` (같은 크기) |
| `-` | 행렬 뺄셈 | `A - B` (같은 크기) |
| `*` | 스칼라 곱 | `A * 2` 또는 `2 * A` |
| `@` | 행렬 곱셈 | `A @ B` (A의 열 = B의 행) |

```finelang
let A = [[1, 2], [3, 4]]
let B = [[5, 6], [7, 8]]

# 행렬 덧셈/뺄셈
let C = A + B          # [[6, 8], [10, 12]]
let D = B - A          # [[4, 4], [4, 4]]

# 스칼라 곱 (교환법칙 지원)
let E = A * 2          # [[2, 4], [6, 8]]
let F = 3 * A          # [[3, 6], [9, 12]]

# 행렬 곱셈
let M1 = [[1, 2], [3, 4]]
let M2 = [[2, 0], [1, 2]]
let M3 = M1 @ M2       # [[4, 4], [10, 8]]

# 복합 연산
let result = (A + B) * 2
```

---

## 4. 제어 구조

### if-else 조건문

```finelang
if condition {
    # condition이 true일 때 실행
} else {
    # condition이 false일 때 실행
}
```

**예제:**

```finelang
let score = 85

if score >= 90 {
    print("Grade: A")
} else {
    if score >= 80 {
        print("Grade: B")
    } else {
        print("Grade: C")
    }
}
```

### for 루프

```finelang
for iterator in iterable {
    # 반복 실행할 코드
}
```

**예제:**

```finelang
# 숫자 범위로 반복
for i in range(0, 5) {
    print(i)  # 0, 1, 2, 3, 4
}

# 배열로 반복
for num in [10, 20, 30] {
    print(num)
}
```

### while 루프

```finelang
while condition {
    # condition이 true인 동안 반복
}
```

**예제:**

```finelang
let i = 0
while i < 5 {
    print(i)
    i = i + 1
}
```

### parallel for (병렬 처리)

```finelang
parallel for i in range(0, 1000) {
    # 병렬로 실행되는 코드
    # 대량의 데이터 처리에 유용
}
```

---

## 5. 함수

### 함수 정의

```finelang
fn function_name(param1, param2) {
    # 함수 본문
    return value
}
```

**예제:**

```finelang
fn add(a, b) {
    return a + b
}

fn greet(name) {
    print("Hello,", name, "!")
}

let result = add(5, 3)
greet("Alice")
```

### 반환값이 없는 함수

```finelang
fn printInfo(name, age) {
    print(name, "is", age, "years old")
}

printInfo("Bob", 25)
```

---

## 6. 자료 구조

### 배열 (Array)

#### 생성과 접근

```finelang
let numbers = [1, 2, 3, 4, 5]
print(numbers[0])     # 1
print(numbers[2])     # 3
```

#### 중첩 배열

```finelang
let nested = [
    [1, 2, 3],
    [4, 5, 6],
    [7, 8, 9]
]

print(nested[0][1])   # 2
print(nested[1][2])   # 6
```

### 행렬 (Matrix) - v2.1.0+

#### 행렬 생성
중첩 배열이 모든 행의 길이가 같으면 자동으로 행렬로 변환됩니다.

```finelang
# 2x3 행렬
let A = [[1, 2, 3], [4, 5, 6]]

# 3x3 단위행렬
let I = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
```

#### 행렬 출력

```finelang
print(A)
# Matrix(2x3)[
#   [1, 2, 3],
#   [4, 5, 6]
# ]
```

#### 행렬 인덱싱

```finelang
let M = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]

# 1차원 인덱싱 (행 접근)
print(M[0])    # [1, 2, 3] - 첫 번째 행
print(M[1])    # [4, 5, 6] - 두 번째 행

# 2차원 인덱싱 (개별 요소 접근)
print(M[0][0])  # 1
print(M[0][2])  # 3
print(M[1][1])  # 5
print(M[2][2])  # 9
```

#### 행렬 순회

```finelang
let M = [[1, 2], [3, 4]]

# 모든 요소 출력
let i = 0
while i < 2 {
    let j = 0
    while j < 2 {
        print(M[i][j])
        j = j + 1
    }
    i = i + 1
}
```

#### 행렬 연산 (v2.2.0+)

```finelang
let A = [[1, 2], [3, 4]]
let B = [[5, 6], [7, 8]]

# 덧셈
let C = A + B    # [[6, 8], [10, 12]]

# 뺄셈
let D = B - A    # [[4, 4], [4, 4]]

# 스칼라 곱
let E = A * 2    # [[2, 4], [6, 8]]
let F = 3 * A    # [[3, 6], [9, 12]]

# 행렬 곱셈 (@ 연산자)
let M1 = [[1, 2], [3, 4]]
let M2 = [[2, 0], [1, 2]]
let M3 = M1 @ M2 # [[4, 4], [10, 8]]

# 복합 연산
let result = (A + B) * 2
```

### 딕셔너리 (Dictionary)

#### 생성과 접근

```finelang
let person = {name: "Alice", age: 30, city: "Seoul"}

print(person["name"])   # Alice
print(person["age"])    # 30
```

#### 중첩 자료 구조

```finelang
# 딕셔너리 안의 배열
let team = {
    name: "Dev Team",
    members: ["Alice", "Bob", "Charlie"],
    size: 3
}

print(team["members"])       # [Alice, Bob, Charlie]
print(team["members"][0])    # Alice

# 배열 안의 딕셔너리
let users = [
    {name: "Alice", role: "Engineer"},
    {name: "Bob", role: "Designer"}
]

print(users[0]["name"])      # Alice
print(users[1]["role"])      # Designer
```

#### 딕셔너리 내장 함수

```finelang
let data = {x: 10, y: 20, z: 30}

# keys(dict) - 모든 키 반환
let key_list = keys(data)       # [x, y, z]

# values(dict) - 모든 값 반환
let val_list = values(data)     # [10, 20, 30]

# len(dict) - 크기 반환
print(len(data))                # 3
```

---

## 7. 객체 지향 프로그래밍

### 클래스 정의 (v1.2.0+)

```finelang
class ClassName {
    let field1
    let field2
    
    fn constructor(param1, param2) {
        this.field1 = param1
        this.field2 = param2
    }
    
    fn method() {
        # 메서드 구현
    }
}
```

### 기본 예제

```finelang
class Person {
    let name
    let age
    
    fn constructor(n, a) {
        this.name = n
        this.age = a
    }
    
    fn greet() {
        print("Hello, I'm", this.name)
    }
    
    fn info() {
        print(this.name, "is", this.age, "years old")
    }
}

# 인스턴스 생성
let alice = new Person("Alice", 30)
alice.greet()   # Hello, I'm Alice
alice.info()    # Alice is 30 years old
```

### 필드 접근과 수정 (v1.3.0+)

```finelang
class Counter {
    let value
    
    fn constructor() {
        this.value = 0
    }
    
    fn increment() {
        this.value = this.value + 1
    }
    
    fn getValue() {
        return this.value
    }
}

let counter = new Counter()
counter.increment()
counter.increment()
print(counter.getValue())  # 2
```

### 상속 (v1.4.0+)

```finelang
class Animal {
    let name
    
    fn constructor(n) {
        this.name = n
    }
    
    fn speak() {
        print(this.name, "makes a sound")
    }
}

class Dog extends Animal {
    let breed
    
    fn constructor(n, b) {
        super(n)  # 부모 클래스 생성자 호출
        this.breed = b
    }
    
    fn speak() {
        print(this.name, "barks!")
    }
    
    fn info() {
        print(this.name, "is a", this.breed)
    }
}

let dog = new Dog("Max", "Golden Retriever")
dog.speak()  # Max barks!
dog.info()   # Max is a Golden Retriever
```

### super 키워드

```finelang
class Shape {
    let color
    
    fn constructor(c) {
        this.color = c
    }
    
    fn describe() {
        print("A", this.color, "shape")
    }
}

class Circle extends Shape {
    let radius
    
    fn constructor(c, r) {
        super(c)
        this.radius = r
    }
    
    fn describe() {
        super.describe()  # 부모 메서드 호출
        print("with radius", this.radius)
    }
}

let circle = new Circle("red", 5)
circle.describe()
# A red shape
# with radius 5
```

---

## 8. 예외 처리

### 스택 추적 (v1.7.0+)

모든 예외 발생 시 자동으로 함수 호출 스택이 추적됩니다:

```finelang
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

**특징:**
- Python 스타일의 `Traceback (most recent call last):` 포맷
- 각 프레임에 파일 이름, 라인 번호, 함수 이름 표시
- 가장 최근 호출이 마지막에 표시됨
- 최대 스택 깊이: 1000 프레임 (초과 시 RecursionError)

### 재귀 깊이 제한 (v1.7.0+)

무한 재귀를 방지하기 위한 스택 깊이 제한:

```finelang
fn recursive(n) {
    if n > 0 {
        recursive(n + 1)
    }
}

recursive(1)

# 출력 (일부):
# Traceback (most recent call last):
#   File "<input>", line 3, in recursive
#   File "<input>", line 3, in recursive
#   ... (1000 frames)
# RecursionError: maximum recursion depth exceeded
```

### try-catch 블록 (v1.5.0+)

```finelang
try {
    # 예외가 발생할 수 있는 코드
    let result = 10 / 0
} catch e {
    # 예외 처리
    print("Error:", e)
}
```

### throw 문

```finelang
throw "Error message"
```

### finally 블록

```finelang
try {
    # 작업 수행
    let result = someFunction()
} catch e {
    print("Error:", e)
} finally {
    # 항상 실행되는 정리 코드
    print("Cleanup complete")
}
```

### 타입별 catch (v1.6.0+)

특정 예외 타입만 잡을 수 있습니다:

```finelang
# ZeroDivisionError만 잡기
try {
    let x = 10 / 0
} catch ZeroDivisionError as e {
    print("Caught division by zero:", e)
}

# IndexError만 잡기
try {
    let arr = [1, 2, 3]
    print(arr[10])
} catch IndexError as e {
    print("Caught index error:", e)
}

# 타입이 맞지 않으면 예외는 계속 전파됨
try {
    try {
        let x = 10 / 0
    } catch IndexError as e {
        print("This won't execute")
    }
} catch e {
    print("Caught by outer catch:", e)
}
```

### assert 문 (v1.6.0+)

조건을 검증하고 실패 시 AssertionError를 발생시킵니다:

```finelang
# 기본 사용법
assert x > 0, "x must be positive"

# 함수에서 전제 조건 검증
fn safe_divide(a, b) {
    assert b != 0, "Divisor cannot be zero"
    return a / b
}

try {
    safe_divide(10, 0)
} catch AssertionError as e {
    print("Assertion failed:", e)
}

# 여러 조건 검증
fn safe_access(arr, index) {
    assert index >= 0, "Index must be non-negative"
    assert index < len(arr), "Index out of bounds"
    return arr[index]
}
```

### 내장 예외 타입

#### 1. ZeroDivisionError
**0으로 나누기 시도**

```finelang
try {
    let x = 10 / 0
} catch ZeroDivisionError as e {
    print("Caught:", e)
    # Output: ZeroDivisionError: division by zero
}
```

#### 2. IndexError
**배열 인덱스 범위 초과**

```finelang
try {
    let arr = [1, 2, 3]
    print(arr[10])
} catch IndexError as e {
    print("Caught:", e)
    # Output: IndexError: list index out of range: 10
}
```

#### 3. KeyError
**딕셔너리 키 없음**

```finelang
try {
    let person = {name: "Alice", age: 30}
    print(person["city"])
} catch KeyError as e {
    print("Caught:", e)
    # Output: KeyError: key not found: 'city'
}
```

#### 4. RuntimeError
**일반 런타임 오류 (throw 문)**

```finelang
try {
    throw "Something went wrong"
} catch e {
    print("Caught:", e)
    # Output: RuntimeError: Something went wrong
}
```

#### 5. AssertionError (v1.6.0+)
**assert 문 실패**

```finelang
try {
    assert 0, "This will fail"
} catch AssertionError as e {
    print("Caught:", e)
    # Output: AssertionError: This will fail
}
```

#### 6. RecursionError (v1.7.0+)
**재귀 깊이 제한 초과**

```finelang
fn infinite_recursion(n) {
    infinite_recursion(n + 1)
}

try {
    infinite_recursion(0)
} catch RecursionError as e {
    print("Caught:", e)
    # Output: RecursionError: maximum recursion depth exceeded
}
```

**참고:** 최대 스택 깊이는 1000 프레임으로 설정되어 있습니다.

### 함수에서의 예외 처리

```finelang
fn divide(a, b) {
    if b == 0 {
        throw "Cannot divide by zero"
    }
    return a / b
}

try {
    let result = divide(10, 0)
    print("Result:", result)
} catch e {
    print("Error:", e)
}
```

### 예외 전파

```finelang
fn level3() {
    throw "Error at level 3"
}

fn level2() {
    level3()  # 예외가 여기로 전파
}

fn level1() {
    try {
        level2()
    } catch e {
        print("Caught at level 1:", e)
    }
}

level1()  # Caught at level 1: RuntimeError: Error at level 3
```

---

## 9. 내장 함수

### 입출력

#### print(...args)
여러 값을 출력합니다.

```finelang
print("Hello")
print("x =", 42)
print([1, 2, 3])
```

### 배열 함수

#### range(start, end)
start부터 end-1까지의 숫자 배열을 생성합니다.

```finelang
let numbers = range(0, 5)    # [0, 1, 2, 3, 4]
let nums = range(5, 10)      # [5, 6, 7, 8, 9]
```

#### len(array | dict)
배열이나 딕셔너리의 크기를 반환합니다.

```finelang
let count = len([1, 2, 3, 4, 5])       # 5
let size = len({x: 10, y: 20})         # 2
```

#### sum(array)
배열의 모든 요소의 합을 반환합니다.

```finelang
let total = sum([1, 2, 3, 4, 5])       # 15
```

#### push(array, value)
배열 끝에 요소를 추가합니다.

```finelang
let arr = [1, 2, 3]
push(arr, 4)
print(arr)  # [1, 2, 3, 4]
```

### 딕셔너리 함수

#### keys(dict)
딕셔너리의 모든 키를 배열로 반환합니다.

```finelang
let data = {x: 10, y: 20, z: 30}
let key_list = keys(data)  # [x, y, z]
```

#### values(dict)
딕셔너리의 모든 값을 배열로 반환합니다.

```finelang
let data = {x: 10, y: 20, z: 30}
let val_list = values(data)  # [10, 20, 30]
```

### 타입 체크 함수 (v2.2.5)

#### is_null(value)
값이 null인지 확인합니다.

```finelang
let x = null
if is_null(x) {
    print("x는 null입니다")
}

print(is_null(null))  # 1 (true)
print(is_null(42))    # 0 (false)
```

#### is_number(value)
값이 숫자인지 확인합니다.

```finelang
print(is_number(42))      # 1
print(is_number(3.14))    # 1
print(is_number("hello")) # 0
```

#### is_string(value)
값이 문자열인지 확인합니다.

```finelang
print(is_string("hello")) # 1
print(is_string(42))      # 0
```

#### is_bool(value)
값이 boolean(0 또는 1)인지 확인합니다.

```finelang
print(is_bool(1))   # 1
print(is_bool(0))   # 1
print(is_bool(2))   # 0
```

#### is_array(value)
값이 배열인지 확인합니다.

```finelang
print(is_array([1, 2, 3]))  # 1
print(is_array({x: 10}))    # 0
```

#### is_dict(value)
값이 딕셔너리인지 확인합니다.

```finelang
print(is_dict({x: 10, y: 20}))  # 1
print(is_dict([1, 2, 3]))       # 0
```

#### is_matrix(value)
값이 행렬인지 확인합니다.

```finelang
let m = [[1, 2], [3, 4]]
print(is_matrix(m))        # 1
print(is_matrix([1, 2]))   # 0
```

#### typeof(value)
값의 자료형을 문자열로 반환합니다.

```finelang
print(typeof(null))        # "null"
print(typeof(42))          # "number"
print(typeof("hello"))     # "string"
print(typeof([1, 2]))      # "array"
print(typeof({x: 10}))     # "dict"
print(typeof([[1, 2]]))    # "matrix"
print(typeof(fn() {}))     # "function"
```

**타입 안전 함수 예제**

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

print(safe_divide(10, 2))     # 5
print(safe_divide(10, 0))     # Error: division by zero, null
print(safe_divide(10, null))  # Error: b is null, null
```

### 고차 함수 (v1.8.0)

#### map(function, array)
배열의 각 요소에 함수를 적용하여 새 배열을 생성합니다.

```finelang
fn double(x) {
    return x * 2
}

let numbers = [1, 2, 3, 4, 5]
let doubled = map(double, numbers)  # [2, 4, 6, 8, 10]

# 익명 함수처럼 사용 가능
fn square(x) { return x * x }
let squared = map(square, [1, 2, 3])  # [1, 4, 9]
```

**파라미터:**
- `function`: 각 요소에 적용할 함수 (파라미터 1개)
- `array`: 변환할 배열

**반환값:** 함수를 적용한 결과로 이루어진 새 배열

#### filter(predicate, array)
조건을 만족하는 요소만 포함하는 새 배열을 생성합니다.

```finelang
fn is_positive(x) {
    return x > 0
}

let numbers = [-2, -1, 0, 1, 2]
let positive = filter(is_positive, numbers)  # [1, 2]

# 복잡한 조건도 가능
fn is_greater_than_5(x) {
    return x > 5
}
let large = filter(is_greater_than_5, [1, 3, 5, 7, 9])  # [7, 9]
```

**파라미터:**
- `predicate`: 조건을 검사하는 함수 (파라미터 1개, truthy 값 반환)
- `array`: 필터링할 배열

**반환값:** 조건을 만족하는 요소들로 이루어진 새 배열

#### reduce(function, array, initial)
배열의 모든 요소를 하나의 값으로 축약합니다.

```finelang
fn add(accumulator, current) {
    return accumulator + current
}

let numbers = [1, 2, 3, 4, 5]
let sum = reduce(add, numbers, 0)  # 15

# 곱셈도 가능
fn multiply(acc, x) {
    return acc * x
}
let product = reduce(multiply, [2, 3, 4], 1)  # 24
```

**파라미터:**
- `function`: 축약 함수 (파라미터 2개: accumulator, current_value)
- `array`: 축약할 배열
- `initial`: 초기값 (accumulator의 시작값)

**반환값:** 최종 축약된 단일 값

**동작 방식:**
1. accumulator = initial
2. 배열의 각 요소에 대해:
   - accumulator = function(accumulator, current_element)
3. 최종 accumulator 반환

#### 함수 체이닝

여러 고차 함수를 조합하여 복잡한 변환을 수행할 수 있습니다.

```finelang
fn square(x) { return x * x }
fn is_greater_than_5(x) { return x > 5 }
fn add(acc, x) { return acc + x }

let numbers = [1, 2, 3, 4, 5]

# [1,2,3,4,5] -> square -> [1,4,9,16,25] -> filter(>5) -> [9,16,25] -> sum -> 50
let result = reduce(add, filter(is_greater_than_5, map(square, numbers)), 0)
print(result)  # 50
```

---

### 유틸리티 함수 (v2.2.6)

> ✨ **Python 스타일 메서드 체이닝 지원!**  
> 모든 유틸리티 함수는 `.` 연산자로 메서드처럼 호출 가능합니다.

#### 사용 방법

유틸리티 함수는 두 가지 방식으로 사용할 수 있습니다:

1. **함수 호출 방식** (기존): `contains(arr, value)`
2. **메서드 체이닝 방식** (Python 스타일): `arr.contains(value)`

```finelang
# 두 방식 모두 동일하게 작동
let arr = [1, 2, 3, 4, 5]

# 함수 호출 방식
print(contains(arr, 3))      # 1

# 메서드 체이닝 방식
print(arr.contains(3))       # 1

# 메서드 체이닝이 더 직관적!
print([1, 2, 3].reverse())   # [3, 2, 1]
```

#### array.contains(value) / contains(array, value)
배열이나 문자열에 특정 값이 포함되어 있는지 확인합니다.

```finelang
# 배열 검색 - 메서드 방식
let arr = [1, 2, 3, 4, 5]
print(arr.contains(3))       # 1 (true)
print(arr.contains(10))      # 0 (false)

# 문자열 검색 - 메서드 방식
print("Hello, World!".contains("World"))  # 1
print("Hello".contains("xyz"))            # 0

# 함수 방식도 여전히 작동
print(contains(arr, 3))                   # 1
```

**파라미터:**
- `value`: 찾을 값 (배열의 경우 모든 타입, 문자열의 경우 부분 문자열)

**반환값:** 1 (포함됨) 또는 0 (포함되지 않음)

#### array.index_of(value) / index_of(array, value)
배열이나 문자열에서 특정 값의 인덱스를 반환합니다.

```finelang
# 배열 인덱스 - 메서드 방식
let arr = [10, 20, 30, 40]
print(arr.index_of(30))       # 2
print(arr.index_of(99))       # -1 (not found)

# 문자열 인덱스 - 메서드 방식
print("Hello".index_of("l"))  # 2 (첫 번째 'l')
print("Hello".index_of("lo")) # 3
print("Hello".index_of("x"))  # -1

# 함수 방식
print(index_of(arr, 30))      # 2
```

**파라미터:**
- `value`: 찾을 값

**반환값:** 0-based 인덱스 또는 -1 (찾지 못함)

#### array.append(value) / append(array, value)
배열의 끝에 새 요소를 추가합니다.

```finelang
# 메서드 방식 (Python 스타일!)
let arr = [1, 2, 3]
let new_arr = arr.append(4)
print(arr)      # [1, 2, 3] (원본 유지)
print(new_arr)  # [1, 2, 3, 4]

# 함수 방식
let arr2 = append([1, 2], 3)
print(arr2)  # [1, 2, 3]

# 체이닝도 가능
let mixed = [].append(42).append("hello")
print(mixed)  # [42, "hello"]
```

**파라미터:**
- `value`: 추가할 값 (모든 타입 가능)

**반환값:** 요소가 추가된 새 배열

**주의:** 원본 배열은 수정되지 않습니다 (불변성 유지).

#### array.reverse() / reverse(array)
배열을 역순으로 뒤집습니다.

```finelang
# 메서드 방식
let arr = [1, 2, 3, 4, 5]
let rev = arr.reverse()
print(rev)  # [5, 4, 3, 2, 1]

# 인라인 사용
print([1, 2, 3].reverse())  # [3, 2, 1]

# 함수 방식
let words = ["apple", "banana", "cherry"]
print(reverse(words))  # ["cherry", "banana", "apple"]
```

**파라미터:** 없음

**반환값:** 역순으로 정렬된 새 배열

#### array.min() / min(array)
배열에서 최솟값을 찾습니다.

```finelang
# 메서드 방식
let numbers = [5, 2, 8, 1, 9, 3]
print(numbers.min())  # 1

# 함수 방식
print(min([5, 2, 8]))  # 2

# 음수도 가능
let temps = [-5, 3, -2, 0, 7]
print(temps.min())  # -5
```

**파라미터:** 없음

**반환값:** 배열의 최솟값

**오류:** 빈 배열이거나 숫자가 아닌 요소가 있으면 에러

#### array.max() / max(array)
배열에서 최댓값을 찾습니다.

```finelang
# 메서드 방식
let numbers = [5, 2, 8, 1, 9, 3]
print(numbers.max())  # 9

# 실수도 가능
let scores = [85.5, 92.3, 78.9, 95.0]
print(scores.max())  # 95

# 함수 방식
print(max([1, 5, 3]))  # 5
```

**파라미터:** 없음

**반환값:** 배열의 최댓값

**오류:** 빈 배열이거나 숫자가 아닌 요소가 있으면 에러

#### 실용 예제 - 메서드 체이닝

```finelang
# 점수 분석 시스템 (Python 스타일!)
fn analyze_scores(scores) {
    let highest = scores.max()
    let lowest = scores.min()
    
    print("Highest score:", highest)
    print("Lowest score:", lowest)
    print("Range:", highest - lowest)
    
    if scores.contains(100) {
        print("Perfect score achieved!")
    }
    
    # 메서드 체이닝으로 간결하게
    let fail_threshold = 60
    let passing = []
    let i = 0
    while i < len(scores) {
        if scores[i] >= fail_threshold {
            let passing = passing.append(scores[i])
        }
        let i = i + 1
    }
    
    print("Passing scores:", passing)
    print("Pass rate:", len(passing), "/", len(scores))
}

let exam_scores = [85, 92, 78, 95, 88, 100, 65, 55]
analyze_scores(exam_scores)
# Output:
# Highest score: 100
# Lowest score: 55
# Range: 45
# Perfect score achieved!
# Passing scores: [85, 92, 78, 95, 88, 100, 65]
# Pass rate: 7 / 8
```
```

### 타입 변환

#### str(value)
값을 문자열로 변환합니다.

```finelang
let num = 42
let text = str(num)  # "42"
```

---

## 활용 예제

### AI/ML 모델 설정

```finelang
let model_config = {
    learning_rate: 0.001,
    batch_size: 32,
    epochs: 100,
    optimizer: "Adam"
}

print("Learning rate:", model_config["learning_rate"])
print("Batch size:", model_config["batch_size"])
```

### 벡터 계산 (선형 대수)

```finelang
let weights = [0.5, 0.3, 0.2]
let inputs = [1.0, 2.0, 3.0]

# 가중 합
let weighted_sum = weights @ inputs
print("Weighted sum:", weighted_sum)  # 1.7
```

### 안전한 파일 처리

```finelang
fn processFile(filename) {
    try {
        # 파일 처리 로직
        let data = loadFile(filename)
        return data
    } catch e {
        print("File error:", e)
        return null
    } finally {
        print("File processing complete")
    }
}
```

### 데이터 검증 클래스

```finelang
class DataValidator {
    let min_value
    let max_value
    
    fn constructor(min, max) {
        this.min_value = min
        this.max_value = max
    }
    
    fn validate(value) {
        if value < this.min_value {
            throw "Value too small"
        }
        if value > this.max_value {
            throw "Value too large"
        }
        return 1  # Valid
    }
}

let validator = new DataValidator(0, 100)

try {
    validator.validate(150)
} catch e {
    print("Validation error:", e)
}
```

---

## 버전별 주요 기능

| 버전 | 주요 기능 |
|------|----------|
| v1.0.0 | 기본 문법, 변수, 함수, 제어문, 배열, 벡터 연산 |
| v1.1.0 | 딕셔너리 (해시맵) |
| v1.2.0 | 클래스, 메서드, new 키워드 |
| v1.3.0 | 필드 접근/수정, this, 생성자 |
| v1.4.0 | 상속, super, 메서드 오버라이딩 |
| v1.5.0 | 예외 처리 (try/catch/throw/finally) |
| v1.6.0 | 타입별 catch, assert 문 |
| v1.7.0 | 스택 추적 (Stack Trace) |
| v1.8.0 | 고차 함수 (map, filter, reduce) |
| v1.9.0 | 모듈 시스템 (import/export) |
| v2.0.0 | 직접 메서드 호출 (module.function()) |
| v2.1.0 | 행렬 (Matrix) 타입 |
| v2.2.0 | 행렬 연산 (+, -, *, @) |
| v2.2.1 | 2차원 인덱싱 (m[i][j]) |
| v2.2.2 | from...import 구문 |
| v2.2.3 | 모듈 별칭 (as) |
| v2.2.4 | 나머지(%), 몫(//) 연산자 |
| v2.2.5 | 타입 체크 함수 (is_null, typeof 등) |
| v2.2.6 | 유틸리티 함수 (contains, append, min/max 등) |

---

## 10. 모듈 시스템

> ✨ **v1.9.0에서 추가됨**

FineLang은 `import`와 `export` 키워드를 사용하여 코드를 모듈화할 수 있습니다.

### 10.1 모듈 만들기 (export)

함수나 변수를 모듈 외부로 내보낼 수 있습니다.

**파일: mymath.fine**
```finelang
# 함수 내보내기
export fn abs(x) {
    if x < 0 {
        return -x
    }
    return x
}

export fn max(a, b) {
    if a > b {
        return a
    }
    return b
}

# 상수 내보내기
export let PI = 3.14159265359
export let E = 2.71828182846

# 내부 전용 함수 (export 없음)
fn internal_helper() {
    print("This is not exported")
}
```

### 10.2 모듈 가져오기 (import)

다른 파일에서 모듈을 가져올 수 있습니다.

**파일: main.fine**
```finelang
import mymath

# v1.9.0에서는 기본 import만 지원
# 모듈이 로드되고 export된 내용이 실행됩니다
```

### 10.3 표준 라이브러리

FineLang은 `stdlib/` 디렉토리에 표준 라이브러리 모듈을 제공합니다.

#### stdlib/math.fine

```finelang
import math

# 제공되는 함수:
# - abs(x): 절대값
# - max(a, b): 최댓값
# - min(a, b): 최솟값
# - pow(base, exp): 거듭제곱
# - factorial(n): 팩토리얼
#
# 제공되는 상수:
# - PI: 3.14159265359
# - E: 2.71828182846
```

#### stdlib/string.fine

```finelang
import string

# 제공되는 함수:
# - length(s): 문자열 길이
# - repeat(s, n): 문자열 n번 반복
# - is_empty(s): 문자열이 비어있는지 확인
```

### 10.4 모듈 검색 경로

FineLang은 다음 순서로 모듈을 검색합니다:

1. **표준 라이브러리**: `stdlib/모듈명.fine`
2. **현재 디렉토리**: `./모듈명.fine`

```finelang
import math      # stdlib/math.fine을 찾음
import mymodule  # ./mymodule.fine을 찾음
```

### 10.5 from...import 구문 (v2.2.2+)

모듈에서 특정 심볼만 가져와서 직접 사용할 수 있습니다:

```finelang
# math 모듈에서 특정 함수만 가져오기
from math import abs, max, PI

print(abs(-5))      # 5 - math. 없이 직접 사용
print(max(10, 20))  # 20
print(PI)           # 3.14159

# 여러 줄로 나누기
from string import length, 
                   repeat,
                   is_empty
```

**일반 import vs from...import 비교**

```finelang
# 방법 1: 일반 import (네임스페이스 접근)
import math
print(math.abs(-5))  # math. 접두사 필요

# 방법 2: from...import (직접 접근)
from math import abs
print(abs(-5))       # 직접 사용

# 방법 3: 별칭 사용
import math as m
print(m.abs(-5))     # 짧은 별칭 사용
```

### 10.6 모듈 별칭 (v2.2.3+)

모듈을 짧은 이름이나 의미있는 이름으로 별칭 지정할 수 있습니다:

```finelang
# 긴 모듈 이름을 짧게
import math as m
print(m.abs(-42))    # 42
print(m.PI)          # 3.14159

# 의미있는 별칭
import math as mathematics
print(mathematics.max(10, 20))  # 20

# 같은 모듈을 여러 별칭으로 (테스트용)
import math as m1
import math as m2
print(m1.abs(-1))    # 1
print(m2.abs(-2))    # 2
```

**별칭 사용 시나리오**

1. **긴 모듈 이름 단축**
   ```finelang
   import very_long_module_name as vlmn
   ```

2. **네이밍 충돌 방지**
   ```finelang
   import utils as my_utils
   import other_lib as other
   ```

3. **코드 가독성 향상**
   ```finelang
   import math as m
   # m.sin(), m.cos() - 짧고 명확
   ```

**from...import와 별칭 혼용**

```finelang
# 둘 다 사용 가능
import math as m
from math import PI

print(m.abs(-5))     # 별칭으로 접근
print(PI)            # 직접 접근
```

### 10.7 v2.0의 제한사항

### 10.8 모듈 예제

**완전한 예제: 사용자 정의 유틸리티 모듈**

**파일: utils.fine**
```finelang
export fn greet(name) {
    print("안녕하세요,", name, "님!")
}

export fn add(a, b) {
    return a + b
}

export fn multiply(a, b) {
    return a * b
}

export let VERSION = "1.0.0"
export let AUTHOR = "FineLang Team"
```

**파일: app.fine**
```finelang
import utils

print("=== Utils 모듈 테스트 ===")
# v2.0에서 utils.greet("사용자") 형태로 호출 가능
# v2.0에서 utils.add(10, 20) 형태로 호출 가능
```

---

## 추가 정보

- **GitHub**: [FineLang 저장소](https://github.com/yourusername/finelang)
- **변경 사항**: CHANGELOG.md 참조
- **프로젝트 요약**: PROJECT_SUMMARY.md 참조

---

**FineLang v2.2.6** - AI/ML을 위한 간결하고 강력한 언어 🚀
