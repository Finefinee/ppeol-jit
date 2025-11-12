# FineLang 🚀

> **현재 버전: v2.3.0** | AI/ML에 최적화된 간결하고 강력한 프로그래밍 언어

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
[![Version](https://img.shields.io/badge/version-2.3.0-blue)]()
[![License](https://img.shields.io/badge/license-MIT-green)]()
[![Language](https://img.shields.io/badge/language-C-orange)]()

---

## 🌟 FineLang이란?

FineLang은 **AI/ML 개발**과 **일반 프로그래밍**을 위해 설계된 현대적인 프로그래밍 언어입니다. Python의 간결함과 C의 성능을 결합하여, 데이터 과학자와 개발자 모두에게 친숙하면서도 강력한 도구를 제공합니다.

### 📊 프로젝트 정보

| 항목 | 내용 |
|------|------|
| **현재 버전** | v2.3.0 (2025-01-12) |
| **구현 언어** | C (C99 표준) |
| **코드 라인** | ~6,200 LOC |
| **라이센스** | MIT |
| **주요 용도** | AI/ML, 데이터 과학, 일반 프로그래밍 |
| **실행 방식** | 인터프리터 + **바이트코드 VM** ⚡ |

---

## ✨ 주요 특징

### 🎯 핵심 기능

- ⚡ **네이티브 성능**: C로 구현되어 빠른 실행 속도
- 🚀 **바이트코드 VM**: 스택 기반 가상 머신으로 최적화된 실행 (v2.3.0)
- 🔄 **For 루프**: Python 스타일 `for (item in array)` 구문 지원
- 🧮 **행렬 연산**: 2D 행렬 타입, 선형대수 연산 (+, -, *, @), 2차원 인덱싱
- 📦 **모듈 시스템**: import/export, from...import, as 별칭 완벽 지원
- 🎯 **완전한 OOP**: 클래스, 상속, this/super, 메서드 오버라이딩
- 🛡️ **예외 처리**: try/catch/finally, 타입별 catch, 스택 추적
- 🔍 **타입 체크**: is_null(), is_number(), typeof() 등 타입 안전 함수
- 🔧 **간결한 문법**: Python과 유사하여 배우기 쉬움

### ⚡ 바이트코드 VM (v2.3.0) - NEW!

**스택 기반 가상 머신으로 빠른 실행 속도와 최적화된 메모리 사용**

```finelang
# VM이 자동으로 바이트코드로 컴파일하여 실행
let x = 10
let y = 20
print(x + y)  # 30

# 제어문도 최적화됨
if x < y {
    print("x is smaller")
}

while x < 15 {
    print(x)
    x = x + 1
}
```

**바이트코드 VM 특징:**
- 📦 40+ OpCode로 구성된 효율적인 명령어 세트
- 🔧 스택 기반 실행 (256 스택 크기)
- 🎯 최적화된 점프 명령 (if/while/for)
- 💾 효율적인 메모리 관리
- 🚀 인터프리터 대비 성능 향상

### 🔄 For 루프 (v2.3.0) - NEW!

**Python 스타일의 직관적인 반복문**

```finelang
# 배열 순회
for (x in [1, 2, 3, 4, 5]) {
    print(x)
}
# 출력: 1 2 3 4 5

# 합계 계산
let sum = 0
for (x in [10, 20, 30]) {
    sum = sum + x
}
print(sum)  # 60

# 행렬의 각 행 처리
let matrix = [[1, 2], [3, 4], [5, 6]]
for (row in matrix) {
    print(row)
}
# 출력: [1, 2] [3, 4] [5, 6]
```

### 🧮 풍부한 연산자 (v2.2.4)

```finelang
# 기본 산술
print(10 + 5)    # 15
print(10 - 5)    # 5
print(10 * 5)    # 50
print(10 / 3)    # 3.333...

# 나머지와 몫
print(10 % 3)    # 1 (나머지)
print(10 // 3)   # 3 (몫)

# 행렬 곱셈
let A = [[1, 2], [3, 4]]
let B = [[5, 6], [7, 8]]
print(A @ B)     # 행렬 곱셈
```

### 📊 행렬 지원 (v2.1.0 - v2.2.1)

```finelang
# 자동 행렬 타입 인식
let M = [[1, 2, 3], 
         [4, 5, 6]]

# 2차원 인덱싱
print(M[0][1])   # 2

# 행렬 연산
let A = [[1, 2], [3, 4]]
let B = [[5, 6], [7, 8]]
print(A + B)     # 덧셈
print(A - B)     # 뺄셈
print(2 * A)     # 스칼라 곱
print(A @ B)     # 행렬 곱

# 문자열 연산 (Python 스타일)
print("Hello" + " " + "World")   # 연결
print("Ha" * 3)                  # 반복: "HaHaHa"
print("-" * 20)                  # "--------------------"
```

### 🎯 Boolean 타입 (v2.2.8)

```finelang
# true/false 키워드
let is_ready = true
let is_error = false

print(true)              # 1
print(false)             # 0
print(true == 1)         # 1 (true)
print(false == 0)        # 1 (true)

# 조건문에서 사용
if true {
    print("Always runs")
}

# 함수 반환
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

### 📦 모듈 시스템 (v2.0.0 - v2.2.3)

```finelang
# 전체 모듈 임포트
import math
print(math.abs(-5))      # 5

# 별칭 사용
import math as m
print(m.PI)              # 3.14159

# 선택적 임포트
from math import abs, max, PI
print(abs(-10))          # 10
print(max(5, 10))        # 10
print(PI)                # 3.14159
```

### 🎯 고차 함수 (v1.8.0)

```finelang
let numbers = [1, 2, 3, 4, 5]

# map - 변환
let doubled = map(numbers, fn(x) { return x * 2 })
print(doubled)  # [2, 4, 6, 8, 10]

# filter - 필터링
let evens = filter(numbers, fn(x) { return x % 2 == 0 })
print(evens)    # [2, 4]

# reduce - 집계
let sum = reduce(numbers, fn(acc, x) { return acc + x }, 0)
print(sum)      # 15
```

### 🔍 타입 체크 함수 (v2.2.5)

```finelang
# Null 타입
let x = null  # 값이 없음을 나타냄
print(typeof(x))  # "null"

# null 체크
if is_null(x) {
    print("x는 null입니다")
}

# 함수에서 null 반환
fn safe_get(arr, index) {
    if index < 0 { return null }
    if index >= len(arr) { return null }
    return arr[index]
}

let numbers = [10, 20, 30]
print(safe_get(numbers, 1))   # 20
print(safe_get(numbers, 99))  # null

# 타입 확인
print(is_number(42))      # 1 (true)
print(is_string("hi"))    # 1
print(is_array([1,2,3]))  # 1
print(is_dict({x: 10}))   # 1
print(is_matrix([[1,2]])) # 1

# typeof로 타입 얻기
print(typeof(null))      # "null"
print(typeof(42))        # "number"
print(typeof("hello"))   # "string"
print(typeof([1, 2]))    # "array"
print(typeof({x: 10}))   # "dict"
```

### 🛠️ 유틸리티 함수 (v2.2.6)

**Python 스타일 메서드 체이닝 지원!**

```finelang
# 배열 메서드 체이닝
let arr = [1, 2, 3]
let arr2 = arr.append(4)         # [1, 2, 3, 4] (원본 유지)
print(arr)                       # [1, 2, 3]

# 체이닝 가능
let result = [5, 2, 8, 1, 9].reverse()
print(result)                    # [9, 1, 8, 2, 5]

# 검색 메서드
print([1, 2, 3].contains(2))     # 1 (true)
print([1, 2, 3].index_of(2))     # 1

# 수학 메서드
let nums = [5, 2, 8, 1, 9]
print(nums.min())                # 1
print(nums.max())                # 9

# 문자열 메서드
print("Hello, World!".contains("World"))  # 1
print("Hello, World!".index_of("World"))  # 7

# 실용 예제: 메서드 체이닝
let scores = [85, 92, 78, 95, 88]
if scores.contains(95) {
    print("Perfect score found!")
}
print("Highest:", scores.max())  # 95
print("Lowest:", scores.min())   # 78
```

# 안전한 함수 작성
fn safe_divide(a, b) {
    if is_null(a) { return null }
    if is_null(b) { return null }
    if is_number(a) == 0 { return null }
    if is_number(b) == 0 { return null }
    if b == 0 { return null }
    return a / b
}
```

---

## ⚡ 빠른 시작

### 설치 및 빌드

```bash
# 저장소 클론
git clone https://github.com/Finefinee/ppeol-jit.git
cd finelang

# 빌드
make

# (선택사항) 시스템에 설치
sudo make install
```

### 첫 프로그램 실행

**파일: hello.fine**
```finelang
print("Hello, FineLang! 🚀")
```

**실행 (인터프리터 모드):**
```bash
./finelang hello.fine
# 출력: Hello, FineLang! 🚀
```

**실행 (VM 모드):**
```bash
./finelang --vm hello.fine
# 또는
./finelang -v hello.fine

# 출력:
# === Bytecode Disassembly ===
# ... (바이트코드 디버그 정보) ...
# === Execution ===
# Hello, FineLang! 🚀
```

### 실행 모드 비교

| 모드 | 명령어 | 용도 | 특징 |
|------|--------|------|------|
| **인터프리터** | `./finelang file.fine` | 일반 실행 | 빠른 시작, 직접 실행 |
| **VM** | `./finelang --vm file.fine` | 디버깅, 최적화 확인 | 바이트코드 출력, 성능 분석 |
| **REPL** | `./finelang` | 대화형 테스트 | 즉시 코드 테스트 |

### REPL 모드 사용

```bash
./finelang
>>> let x = 42
>>> print(x)
42
>>> let sum = fn(a, b) { return a + b }
>>> print(sum(10, 20))
30
>>> exit
```

---

## 📖 문서 및 예제

### 📚 문서
- **[SYNTAX_GUIDE.md](SYNTAX_GUIDE.md)** - 완전한 문법 가이드
  - 기본 문법 (변수, 함수, 제어문)
  - 객체지향 프로그래밍 (클래스, 상속)
  - 예외 처리 (try/catch/finally)
  - 고차 함수 (map, filter, reduce)
  - 모듈 시스템 (import/export)
  - 행렬 연산
  
- **[CHANGELOG.md](CHANGELOG.md)** - 버전별 상세 변경사항

### 💡 예제 파일

```
examples/
├── hello.fine              # Hello World
├── fibonacci.fine          # 피보나치 수열
├── linear_regression.fine  # 선형 회귀
├── neural_network.fine     # 신경망 예제
├── matrix_basic.fine       # 행렬 기초
├── matrix_operations.fine  # 행렬 연산
├── module_alias.fine       # 모듈 별칭
└── modulo_floordiv.fine   # 나머지/몫 연산
```

---

## 📁 프로젝트 구조

```
finelang/
├── src/                    # 소스 코드
│   ├── lexer.c/h          # 어휘 분석기 (토큰화)
│   ├── parser.c/h         # 구문 분석기 (AST 생성)
│   ├── interpreter.c/h    # 인터프리터 (실행 엔진)
│   ├── bytecode.c/h       # 바이트코드 시스템 (v2.3.0)
│   ├── compiler.c/h       # AST → 바이트코드 컴파일러 (v2.3.0)
│   ├── vm.c/h             # 가상 머신 (v2.3.0)
│   ├── module.c/h         # 모듈 시스템
│   ├── main.c             # 진입점
│   └── vm_test.c          # VM 테스트 도구
├── stdlib/                 # 표준 라이브러리
│   ├── math.fine          # 수학 함수 (abs, max, min, PI 등)
│   └── string.fine        # 문자열 함수
├── examples/               # 예제 코드
│   ├── test_vm_*.fine     # VM 테스트 파일
│   └── ...
├── build/                  # 빌드 출력
├── Makefile               # 빌드 설정
├── README.md              # 이 문서
├── SYNTAX_GUIDE.md        # 문법 가이드
├── CHANGELOG.md           # 변경 이력
└── VM_COMPLETE.md         # VM 구현 문서 (v2.3.0)
```

---

## 🎯 코드 예제

### Hello World
```finelang
print("Hello, FineLang!")
```

### 변수와 함수
```finelang
# 변수 선언
let name = "FineLang"
let version = 2.2

# 함수 정의
fn greet(name) {
    print("Hello,", name, "!")
}

greet("World")
```

### 클래스와 OOP
```finelang
# 클래스 정의
class Person {
    fn init(name, age) {
        this.name = name
        this.age = age
    }
    
    fn introduce() {
        print("I'm", this.name, ", age", this.age)
    }
}

# 상속
class Student extends Person {
    fn init(name, age, major) {
        super.init(name, age)
        this.major = major
    }
    
    fn introduce() {
        super.introduce()
        print("I study", this.major)
    }
}

let student = new Student("Alice", 20, "Computer Science")
student.introduce()
```

### 예외 처리
```finelang
try {
    let result = 10 / 0
} catch ZeroDivisionError as e {
    print("Error:", e.message)
} catch Exception as e {
    print("Unknown error:", e.message)
} finally {
    print("Cleanup complete")
}
```

### 행렬 연산 (AI/ML)
```finelang
# 행렬 생성
let X = [[1, 2, 3],
         [4, 5, 6]]
let W = [[0.1, 0.2],
         [0.3, 0.4],
         [0.5, 0.6]]

# 행렬 곱셈 (신경망 forward pass)
let Y = X @ W
print("Output:", Y)

# 개별 요소 접근
print("Y[0][0] =", Y[0][0])
```

---

## 📈 버전 히스토리

### 최근 릴리스

| 버전 | 날짜 | 주요 기능 |
|------|------|----------|
| **v2.3.0** | 2025-01-12 | ⚡ 바이트코드 VM + For 루프 |
| **v2.2.8** | 2025-01-10 | ✅ Boolean 타입 (true/false) |
| **v2.2.6** | 2025-01-10 | 🔧 유틸리티 함수 (contains, min, max 등) |
| **v2.2.5** | 2025-01-10 | 🔍 Null 타입 + 타입 체크 함수 |
| **v2.2.4** | 2025-01-09 | ➗ 나머지(%), 몫(//) 연산자 |
| **v2.2.3** | 2025-01-09 | 🏷️ 모듈 별칭 (import as) |
| **v2.2.2** | 2025-01-09 | 📦 from...import 구문 |
| **v2.2.1** | 2025-01-09 | 🎯 2차원 인덱싱 (m[i][j]) |
| **v2.2.0** | 2025-01-09 | ➕ 행렬 연산 (+, -, *, @) |
| **v2.1.0** | 2025-01-09 | 📊 행렬 타입 |
| **v2.0.0** | 2025-01-27 | 📦 모듈 시스템 |
| v1.9.0 | 2025-01-27 | 모듈 기초 구조 |
| v1.8.0 | 2025-01-27 | 고차 함수 (map, filter, reduce) |
| v1.7.0 | 2025-01-26 | 스택 추적 |

*전체 변경사항은 [CHANGELOG.md](CHANGELOG.md) 참조*

---

## 🛠️ 개발

### 빌드 명령어

```bash
# 전체 빌드
make

# 클린 빌드
make clean
make

# 디버그 빌드
make debug

# 설치
sudo make install

# 제거
sudo make uninstall
```

### 테스트 실행

```bash
# 기본 테스트
./finelang examples/hello.fine

# 행렬 테스트
./finelang examples/matrix_operations.fine

# 모듈 테스트
./finelang test_from_import.fine

# 연산자 테스트
./finelang test_modulo_floordiv.fine
```

---

## 🗺️ 로드맵

### v2.4.0 - 행렬 유틸리티 (예정)
- [ ] shape() - 행렬 크기
- [ ] transpose() - 전치
- [ ] dot() - 내적
- [ ] eye() - 단위 행렬
- [ ] zeros(), ones() - 특수 행렬

### v2.5.0 - VM 최적화 (계획 중)
- [ ] 함수 정의/호출 지원
- [ ] 클래스 시스템 통합
- [ ] 최적화 패스
- [ ] 성능 벤치마크

### v3.0.0 - JIT 컴파일러 (장기 계획)
- [ ] LLVM 기반 JIT 컴파일
- [ ] 타입 추론
- [ ] 고급 최적화

### 향후 계획
- [ ] GPU 가속 지원
- [ ] 패키지 매니저
- [ ] NumPy/TensorFlow 호환 레이어

---

## 🤝 기여

FineLang은 오픈소스 프로젝트입니다. 기여를 환영합니다!

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

---

## � 라이센스

MIT License - 자세한 내용은 [LICENSE](LICENSE) 파일 참조

---

## 👤 개발자

**Finefinee**
- GitHub: [@Finefinee](https://github.com/Finefinee)
- Repository: [ppeol-jit](https://github.com/Finefinee/ppeol-jit)

---

## ⭐ 지원

이 프로젝트가 도움이 되었다면 ⭐ Star를 눌러주세요!

---

<div align="center">

**Made with ❤️ for AI/ML developers**

[Documentation](SYNTAX_GUIDE.md) • [Changelog](CHANGELOG.md) • [Examples](examples/)

