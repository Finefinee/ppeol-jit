CC = gcc
CFLAGS = -Wall -O2 -std=c99
LDFLAGS = -lm

SRC_DIR = src
BUILD_DIR = build
TARGET = finelang

SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/lexer.c \
          $(SRC_DIR)/parser.c \
          $(SRC_DIR)/interpreter.c \
          $(SRC_DIR)/module.c \
          $(SRC_DIR)/bytecode.c \
          $(SRC_DIR)/compiler.c \
          $(SRC_DIR)/vm.c

OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

.PHONY: all clean install test test-vm help

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

test: $(TARGET)
	@echo "Running interpreter tests..."
	./$(TARGET) examples/hello.fine
	./$(TARGET) examples/fibonacci.fine
	./$(TARGET) examples/vector_ops.fine
	@echo "All interpreter tests passed!"

test-vm: $(TARGET)
	@echo "Running VM tests..."
	./$(TARGET) --vm test_vm.fine
	@echo "VM tests passed!"

help:
	@echo "FineLang Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build the finelang executable (default)"
	@echo "  clean    - Remove build artifacts"
	@echo "  install  - Install finelang to /usr/local/bin"
	@echo "  test     - Run interpreter mode tests"
	@echo "  test-vm  - Run VM mode tests"
	@echo ""
	@echo "Usage:"
	@echo "  ./finelang              - Start REPL"
	@echo "  ./finelang file.fine    - Run in interpreter mode"
	@echo "  ./finelang --vm file.fine - Run in VM mode"
	@echo "  test     - Run example programs"
	@echo "  help     - Show this help message"
