# ===================
# Variable definition
# ===================
MAKEFILE := $(lastword $(MAKEFILE_LIST))
DOCUMENTATION := README.md CONTRIBUTING.md

# ----------------
# Folder variables
# ----------------
SRC_DIR := src
HEADER_DIR := include
TEST_DIR := tests

BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
TEST_BUILD_DIR := $(BUILD_DIR)/tests

# ---------------
# Target variable
# ---------------
TARGET := $(BUILD_DIR)/main

# ---------------------
# Source file variables
# ---------------------
TARGET_SRC := $(TARGET:$(BUILD_DIR)/%=$(SRC_DIR)/%.c)
SRCS := $(filter-out $(TARGET_SRC), $(wildcard $(SRC_DIR)/*.c))
HEADERS := $(wildcard $(HEADER_DIR)/*.h)
TEST_SRCS := $(wildcard $(TEST_DIR)/*.c)

# -------------------
# Byproduct variables
# -------------------
TARGET_OBJ := $(TARGET:$(BUILD_DIR)/%=$(OBJ_DIR)/%.o)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS := $(OBJS:.o=.d) $(TEST_SRCS:$(TEST_DIR)/%.c=$(TEST_BUILD_DIR)/%.d)

.SECONDARY: $(OBJS) $(TARGET_OBJ) $(DEPS)

# --------------
# Test variables
# --------------
TEST_TARGETS := $(TEST_SRCS:$(TEST_DIR)/%.c=$(TEST_BUILD_DIR)/%)

# ----------------------
# Distribution variables
# ----------------------
DIST := Abreu-Chaves-Guevara-Ortiz-Yip.tgz

# ---------------------
# Compilation variables
# ---------------------
CC := clang
CFLAGS += -Wall -Wextra -Wpedantic \
					-Wformat=2 -Wno-unused-parameter -Wshadow \
					-Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
					-Wredundant-decls -Wnested-externs -Wmissing-include-dirs
CFLAGS += -std=c11
CPPFLAGS += -I$(HEADER_DIR) -MMD -MP
LDLIBS += -lm

# =================
# Compilation rules
# =================
.PHONY: all all_tests
all: $(TARGET)
all_tests: $(TEST_TARGETS)

.PHONY: dist
dist: $(DIST)

$(TARGET): $(TARGET_OBJ) $(OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

$(TEST_BUILD_DIR)/%: LDLIBS += -lcriterion
$(TEST_BUILD_DIR)/%: $(TEST_DIR)/%.c $(OBJS) | $(TEST_BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# =================
# Distribution rule
# =================
$(DIST): $(TARGET_SRC) $(SRCS) $(HEADERS) $(TEST_SRCS) $(MAKEFILE) $(DOCUMENTATION)
	tar -zcvf $@ $^

# =====================
# Folder creation rules
# =====================
$(BUILD_DIR):
	mkdir $@

$(OBJ_DIR): | $(BUILD_DIR)
	mkdir $@

$(DEPS_DIR): | $(BUILD_DIR)
	mkdir $@

$(TEST_BUILD_DIR): | $(BUILD_DIR)
	mkdir $@

# ========================
# Pseudo-target definition
# ========================
.PHONY: test
test: $(TEST_TARGETS)
	for test_file in $^; do ./$$test_file; done

.PHONY: clean
clean:
	$(RM) -rf $(BUILD_DIR)
	$(RM) -f $(DIST)

.PHONY: install-hooks
install-hooks:
	pre-commit install
	pre-commit install --hook-type commit-msg

.PHONY: run-hooks
run-hooks:
	pre-commit run --all-files

.PHONY: lint
lint:
	clang-tidy $(TARGET_SRC) $(SRCS) $(HEADERS) $(TEST_SRCS)

# -------------
-include $(DEPS)
