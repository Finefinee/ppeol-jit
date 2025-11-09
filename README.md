# FineLang

> **현재 버전: v2.0.0** | AI/ML에 최적화된 간결하고 빠른 프로그래밍 언어

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
[![Version](https://img.shields.io/badge/version-2.0.0-blue)]()
[![License](https://img.shields.io/badge/license-MIT-green)]()

---

## 📊 프로젝트 개요

| 항목 | 내용 |
|------|------|
| **언어 이름** | FineLang |
| **현재 버전** | v2.0.0 |
| **구현 언어** | C (C99 표준) |
| **코드 라인** | ~5,200 LOC |
| **라이센스** | MIT |
| **주요 용도** | AI/ML, 데이터 과학, 일반 프로그래밍 |

---

## 🚀 특징

- ⚡ **빠른 실행**: C로 구현되어 네이티브 속도 제공
- 🧮 **벡터 연산**: 내장 벡터 타입과 연산자 지원
- 🗂️ **자료 구조**: 딕셔너리(해시맵)와 중첩 자료 구조 지원
- 🎯 **완전한 OOP**: 클래스, 상속, this/super, 오버라이딩 지원
- 🛡️ **고급 예외 처리**: try/catch/finally, 타입별 catch, assert 문
- 🔍 **스택 추적**: 예외 발생 시 함수 호출 스택 자동 표시 (v1.7.0)
- 🎯 **고차 함수**: map, filter, reduce 내장 지원 (v1.8.0)
- 📦 **완전한 모듈 시스템**: import/export로 코드 모듈화, 직접 메서드 호출 지원 (v2.0.0)
- 🤖 **AI 친화적**: 선형 회귀, 신경망 등 머신러닝 알고리즘에 최적화
- 🔧 **간결한 문법**: Python과 유사한 읽기 쉬운 문법
- 🚀 **REPL 모드**: 대화형 개발 환경 제공

## ⚡ 빠른 시작

### 설치 및 빌드

```bash
# 저장소 클론
git clone https://github.com/yourusername/finelang.git
cd finelang

# 빌드
make

# (선택사항) 시스템에 설치
sudo make install
```

### 첫 프로그램

**파일: hello.fine**
```finelang
print("Hello, FineLang!")
```

**실행:**
```bash
./finelang hello.fine
```

### REPL 모드

```bash
./finelang
>>> let x = 42
>>> print(x)
42
>>> exit
```

## 📖 문서

- **[SYNTAX_GUIDE.md](SYNTAX_GUIDE.md)** - 완전한 문법 가이드 (기본 문법, 클래스, 예외 처리, 고차 함수 등)
- **[CHANGELOG.md](CHANGELOG.md)** - 버전별 변경사항

## 📁 프로젝트 구조

```
finelang/
├── src/               # 소스 코드
│   ├── lexer.c/h     # 어휘 분석기
│   ├── parser.c/h    # 구문 분석기
│   ├── interpreter.c/h  # 인터프리터
│   ├── environment.c/h  # 환경 관리
│   ├── module.c/h    # 모듈 시스템 (v1.9.0)
│   └── main.c        # 진입점
├── stdlib/           # 표준 라이브러리 (v1.9.0)
│   ├── math.fine     # 수학 함수
│   └── string.fine   # 문자열 함수
├── examples/         # 예제 코드
├── Makefile          # 빌드 설정
├── README.md         # 이 문서
├── SYNTAX_GUIDE.md   # 완전한 문법 가이드
└── CHANGELOG.md      # 버전 변경 이력
```

## 🎯 성능

FineLang은 매우 빠릅니다:

```
피보나치(20): 즉시 계산
10,000 반복: < 16ms
벡터 연산: 네이티브 속도
```

## 🛠️ 기술 스택 및 아키텍처

### 컴파일 파이프라인

```
소스 코드 (.fine)
    ↓
[ Lexer ] ────→ 토큰 스트림 (Tokens)
    ↓
[ Parser ] ───→ 추상 구문 트리 (AST)
    ↓
[ Interpreter ] → 실행 및 결과 출력
```

### 핵심 구성 요소

| 구성 요소 | 설명 | 파일 |
|-----------|------|------|
| **Lexer** | 소스 코드를 토큰으로 분해 | `src/lexer.c/h` |
| **Parser** | 토큰을 AST로 변환 | `src/parser.c/h` |
| **Interpreter** | AST를 순회하며 실행 | `src/interpreter.c/h` |
| **Environment** | 변수 스코프 관리 | `src/environment.c/h` |

### 프로젝트 통계

- **총 코드 라인**: ~4,500 LOC
- **핵심 파일**: 9개 (.c/.h)
- **AST 노드 타입**: 27개
- **토큰 타입**: 42개
- **값 타입**: 8개 (NUMBER, STRING, ARRAY, DICT, FUNCTION, CLASS, INSTANCE, EXCEPTION, NULL)
- **내장 함수**: 13개
- **예외 타입**: 6개

---

## 📋 내장 함수

### 입출력
- `print(...)`: 값 출력

### 배열 함수
- `range(start, end)`: 범위 생성
- `len(array/dict)`: 크기 반환
- `sum(array)`: 배열 합계
- `push(array, value)`: 배열에 추가

### 딕셔너리 함수
- `keys(dict)`: 모든 키
- `values(dict)`: 모든 값

### 고차 함수 (v1.8.0)
- `map(function, array)`: 배열의 각 요소에 함수 적용
- `filter(predicate, array)`: 조건을 만족하는 요소만 선택
- `reduce(function, array, initial)`: 배열을 단일 값으로 축약

자세한 내용은 [SYNTAX_GUIDE.md](SYNTAX_GUIDE.md)를 참조하세요.

---

## 🎨 주요 연산자

**벡터 연산:**
- `+` - 요소별 덧셈
- `-` - 요소별 뺄셈  
- `*` - 요소별 곱셈
- `@` - 내적 (dot product)

**비교 연산:**
- `==`, `!=`, `<`, `<=`, `>`, `>=`

### ✅ 완료된 버전

| 버전 | 날짜 | 주요 기능 |
|------|------|----------|
| v1.0.0 | 2025-01-20 | 기본 문법, 변수, 함수, 제어문, 배열, 벡터 연산 |
| v1.1.0 | 2025-01-22 | 딕셔너리 (해시맵) |
| v1.2.0 | 2025-01-24 | 클래스, 메서드, new 키워드 |
| v1.3.0 | 2025-01-25 | 필드 접근/수정, this, 생성자 |
| v1.4.0 | 2025-01-26 | 상속, super, 메서드 오버라이딩 |
| v1.5.0 | 2025-01-26 | 예외 처리 (try/catch/throw/finally) |
| v1.6.0 | 2025-01-26 | 타입별 catch, assert 문 |
| v1.7.0 | 2025-01-26 | 스택 추적, 재귀 깊이 제한 |
| v1.8.0 | 2025-01-26 | 고차 함수 (map, filter, reduce), 일급 함수 |

### 향후 개발 계획

#### v1.9.0 - 표준 라이브러리 (계획 중)
- 📂 **파일 I/O**: 파일 읽기/쓰기, 디렉토리 작업
- 🔤 **문자열 메서드**: split, join, replace, strip, upper, lower 등
- 🔢 **수학 라이브러리**: 삼각함수, 지수/로그, 상수(PI, E)

#### v2.0.0 - 완전한 OOP
- [ ] 정적 메서드 (static)
- [ ] Private/Public 접근 제어
- [ ] 추상 클래스 (abstract)
- [ ] 인터페이스 (interface)
- [ ] 프로퍼티 (getter/setter)

#### v3.0.0 - 성능 및 확장
- [ ] JIT 컴파일러 (LLVM)
- [ ] GPU 가속
- [ ] 행렬 연산
- [ ] 모듈 시스템
- [ ] 패키지 매니저

---

## 🎉 주요 성과

### v1.8.0 (최신)
- ✅ **고차 함수** 구현: map, filter, reduce
- ✅ **일급 함수** 지원: 함수를 값으로 전달 가능
- ✅ **함수 체이닝** 가능

### v1.7.0
- ✅ **스택 추적(Stack Trace)** 구현
- ✅ Python 스타일 Traceback 출력
- ✅ 재귀 깊이 제한 (1000 프레임)

### 전체 프로젝트
- ✅ **현대적 프로그래밍 언어**의 모든 핵심 기능 구현
- ✅ **OOP 완전 지원**: 클래스, 상속, 다형성
- ✅ **고급 예외 처리**: 타입별 catch, assert, 스택 추적
- ✅ **함수형 프로그래밍**: 고차 함수, 일급 함수
- ✅ **AI/ML 특화**: 벡터 연산, 딕셔너리
- ✅ **4,500+ LOC**의 견고한 인터프리터
- ✅ **포괄적 문서화**: 문법 가이드, 예제, 변경 이력

---

## 📄 라이센스

MIT License - 자유롭게 사용, 수정, 배포할 수 있습니다.

---

**FineLang v1.8.0** - AI를 위한 간결하고 강력한 언어 🚀
