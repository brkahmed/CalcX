CC = gcc
CFLAGS = -std=c99 -Wall -Wextra

SRC = eval.c
OBJ = $(SRC:.c=.o)

.PHONY: all test clean

all: libeval.a

libeval.a: $(OBJ)
	@echo "[EVAL] Creating static library: $@"
	@ar rcs $@ $^

test: libeval.a test_eval.c
	@echo "[EVAL] Building tests..."
	@$(CC) $(CFLAGS) test_eval.c libeval.a -o test_eval -lm
	@echo "[EVAL] Running tests..."
	@if ./test_eval; then \
	    echo "[EVAL] ✅ All eval tests passed!"; \
	else \
	    echo "[EVAL] ❌ Eval tests failed!"; \
	    exit 1; \
	fi

clean:
	@echo "[EVAL] Cleaning..."
	@rm -f *.o *.a test_eval
