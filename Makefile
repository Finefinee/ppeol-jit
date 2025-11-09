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
          $(SRC_DIR)/module.c

OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

.PHONY: all clean install test

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
	@echo "Running tests..."
	./$(TARGET) examples/hello.fine
	./$(TARGET) examples/fibonacci.fine
	./$(TARGET) examples/vector_ops.fine
	@echo "All tests passed!"

.PHONY: help
help:
	@echo "FineLang Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build the finelang interpreter (default)"
	@echo "  clean    - Remove build artifacts"
	@echo "  install  - Install finelang to /usr/local/bin"
	@echo "  test     - Run example programs"
	@echo "  help     - Show this help message"
