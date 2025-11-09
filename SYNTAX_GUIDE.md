# FineLang 문법 가이드

> **FineLang v1.9.0** - AI/ML 작업에 최적화된 간결하고 빠른 프로그래밍 언어

---

## 목차

1. [기본 문법](#1-기본-문법)
2. [데이터 타입](#2-데이터-타입)
3. [연산자](#3-연산자)
4. [제어 구조](#4-제어-구조)
5. [함수](#5-함수)
6. [자료 구조](#6-자료-구조)
7. [객체 지향 프로그래밍](#7-객체-지향-프로그래밍)
8. [예외 처리](#8-예외-처리)
9. [내장 함수](#9-내장-함수)
10. [모듈 시스템](#10-모듈-시스템)

---

## 1. 기본 문법

### 주석

```finelang
# 이것은 주석입니다
let x = 42  # 인라인 주석도 가능
```

### 변수 선언

```finelang
let x = 42
let name = "FineLang"
let pi = 3.14159
let numbers = [1, 2, 3, 4, 5]
```

---

## 2. 데이터 타입

### Number (숫자)
정수와 실수를 모두 지원합니다.

```finelang
let integer = 42
let float = 3.14159
let negative = -10
```

### String (문자열)
큰따옴표(`"`)로 문자열을 생성합니다.

```finelang
let greeting = "Hello, World!"
let language = "FineLang"
```

### Array (배열)
여러 값을 순서대로 저장합니다.

```finelang
let numbers = [1, 2, 3, 4, 5]
let names = ["Alice", "Bob", "Charlie"]
let mixed = [1, "two", 3.0, [4, 5]]

# 인덱싱 (0부터 시작)
let first = numbers[0]    # 1
let second = numbers[1]   # 2
```

### Dictionary (딕셔너리)
키-값 쌍으로 데이터를 저장합니다. (v1.1.0+)

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

```finelang
let a = 10
let b = 3
print(a + b)  # 13
print(a - b)  # 7
print(a * b)  # 30
print(a / b)  # 3
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
let matrix = [
    [1, 2, 3],
    [4, 5, 6],
    [7, 8, 9]
]

print(matrix[0][1])   # 2
print(matrix[1][2])   # 6
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

### 10.5 v1.9.0의 제한사항

현재 버전에서는 다음 기능이 제한됩니다:

#### ❌ 네임스페이스 접근 불가
```finelang
import math
# math.abs(-5)  # 아직 지원하지 않음 (v2.0 예정)
```

#### ❌ 특정 심볼 import 불가
```finelang
# from math import abs, max  # 아직 지원하지 않음 (v2.0 예정)
```

#### ❌ 별칭(as) 기능 미완성
```finelang
# import math as m  # 파싱은 되지만 동작하지 않음 (v2.0 예정)
```

### 10.6 v2.0 로드맵

다음 기능들이 v2.0에서 추가될 예정입니다:

- ✅ **네임스페이스 접근**: `module.function()` 형태 호출
- ✅ **from...import**: 특정 심볼만 가져오기
- ✅ **별칭(as)**: 모듈이나 심볼에 다른 이름 부여
- ✅ **export 접근성**: import한 모듈의 export된 심볼 사용

### 10.7 모듈 예제

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

**FineLang v1.9.0** - AI/ML을 위한 간결하고 강력한 언어 🚀
