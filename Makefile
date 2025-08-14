CC = gcc
CFLAGS = -std=c99 -Wall -Wextra

OUT_DIR = build
SRC = main.c
LIBS = eval/libeval.a

.PHONY: release debug test clean

release: test
	@echo "[PROJECT] Building in release mode..."
	@mkdir -p $(OUT_DIR)
	@$(CC) $(CFLAGS) -O3 $(SRC) $(LIBS) -o $(OUT_DIR)/calc -lm
	@echo "[PROJECT] ✅ Build complete: $(OUT_DIR)/calc"

debug: $(LIBS)
	@echo "[PROJECT] Building in debug mode..."
	@mkdir -p $(OUT_DIR)
	@$(CC) $(CFLAGS) -g -DDEBUG $(SRC) $(LIBS) -o $(OUT_DIR)/calc_debug -lm
	@echo "[PROJECT] ✅ Build complete: $(OUT_DIR)/calc_debug"

test: $(LIBS)
	@echo "[PROJECT] Running eval tests before build..."
	@$(MAKE) -C eval test

$(LIBS):
	@echo "[PROJECT] Building eval library..."
	@$(MAKE) -C eval

clean:
	@echo "[PROJECT] Cleaning..."
	@rm -rf $(OUT_DIR)
	@$(MAKE) -C eval clean
