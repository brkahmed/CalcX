CC = gcc
CFLAGS = -std=c99 -Wall -Wextra
CFLAGS += -I$(REPLXX_DIR)/include   # add replxx include path
LDFLAGS = -lm                       # base link flags
LDLIBS = $(LIBS) -lstdc++ -pthread

# program output
PROGRAM = calc
OUT_DIR = build
SRC = main.c

LIBS = $(EVAL_LIB) $(REPLXX_LIB)

EVAL_LIB = eval/libeval.a

# replxx paths
REPLXX_DIR = replxx
REPLXX_BUILD = $(REPLXX_DIR)/build
REPLXX_LIB = $(REPLXX_BUILD)/libreplxx.a

.PHONY: release debug test clean

release: CFLAGS += -O3
release: test program

debug: CFLAGS += -g -DDEBUG
debug: PROGRAM := $(PROGRAM)_debug
debug: program

program: $(OUT_DIR) $(SRC) $(LIBS)
	@echo "[PROJECT] Building $(PROGRAM)..."
	@$(CC) $(CFLAGS) $(SRC) $(LDLIBS) -o $(OUT_DIR)/$(PROGRAM) $(LDFLAGS)
	@echo "[PROJECT] ✅ Build complete: $(OUT_DIR)/$(PROGRAM)"

test: $(EVAL_LIB)
	@echo "[PROJECT] Running eval tests before build..."
	@$(MAKE) -C eval test

$(EVAL_LIB):
	@echo "[PROJECT] Building eval library..."
	@$(MAKE) -C eval

$(REPLXX_LIB):
	@echo "[REPLXX] Building replxx..."
	@mkdir -p $(REPLXX_BUILD)
	@cd $(REPLXX_BUILD) && cmake -DCMAKE_BUILD_TYPE=Release ..
	@$(MAKE) -C $(REPLXX_BUILD)

$(OUT_DIR):
	@mkdir -p $(OUT_DIR)

clean:
	@echo "[PROJECT] Cleaning..."
	@rm -rf $(OUT_DIR)
	@$(MAKE) -C eval clean
	@# Uncomment if you want to clean replxx as well (slower rebuilds):
	@# $(MAKE) -C $(REPLXX_BUILD) clean
