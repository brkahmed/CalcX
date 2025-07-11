CC = gcc
CFLAGS = -std=c99 -Wall -Wextra
INC = -Iinclude

EVAL_SRC = eval/eval.c
SRC = main.c $(EVAL_SRC)
TEST_SRC = test/test_eval.c
OUT_DIR = build

DEBUG_BIN = $(OUT_DIR)/calc_debug
RELEASE_BIN = $(OUT_DIR)/calc
TEST_BIN = $(OUT_DIR)/test

all: release

debug: CFLAGS += -g -DDEBUG
debug: $(DEBUG_BIN)

release: CFLAGS += -O3
release: $(RELEASE_BIN)

$(DEBUG_BIN): $(SRC)
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $(INC) $^ -o $@ -lm

$(RELEASE_BIN): $(SRC)
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $(INC) $^ -o $@ -lm

test: CFLAGS += -g -DDEBUG
test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_SRC) $(EVAL_SRC)
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $(INC) $^ -o $@ -lm

clean:
	rm -rf $(OUT_DIR)