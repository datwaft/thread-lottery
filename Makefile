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
RESOURCES_DIR := resources

BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
TEST_BUILD_DIR := $(BUILD_DIR)/tests
SRC_BUILD_DIR := $(BUILD_DIR)/src
HEADER_BUILD_DIR := $(BUILD_DIR)/include

# ---------------
# Target variable
# ---------------
TARGET := $(BUILD_DIR)/main

# ---------------------
# Source file variables
# ---------------------
TARGET_SRC := $(TARGET:$(BUILD_DIR)/%=$(SRC_DIR)/%.c)
SRCS := $(shell find $(SRC_DIR) -type f -path '**/*.c' 2> /dev/null)
SRCS := $(filter-out $(TARGET_SRC), $(SRCS))
HEADERS := $(shell find $(HEADER_DIR) -type f -path '**/*.h' 2> /dev/null)
TEST_SRCS := $(shell find $(TEST_DIR) -type f -path '**/*.c' 2> /dev/null)

# -----------------------
# Resource file variables
# -----------------------
RESOURCE_INDEX := application.gresource.xml
RESOURCES := $(shell find $(RESOURCES_DIR) -type f 2> /dev/null)
RESOURCES_OBJ := $(OBJ_DIR)/resources.o
RESOURCES_SRC := $(RESOURCES_OBJ:$(OBJ_DIR)/%.o=$(SRC_BUILD_DIR)/%.c)
RESOURCES_HEADER := $(RESOURCES_OBJ:$(OBJ_DIR)/%.o=$(HEADER_BUILD_DIR)/%.h)

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
DIST := Abreu-Guevara-Ortiz-Yip.tgz

# ---------------------
# Compilation variables
# ---------------------
CC := clang
CFLAGS += -Wall -Wextra -Wpedantic \
					-Wformat=2 -Wno-unused-parameter -Wshadow \
					-Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
					-Wredundant-decls -Wnested-externs -Wmissing-include-dirs
CFLAGS += -Wno-language-extension-token -Wno-overlength-strings -Wno-gnu-pointer-arith
CFLAGS += $(shell pkg-config --cflags gtk+-3.0)
CPPFLAGS += -I$(HEADER_DIR) -I$(HEADER_BUILD_DIR) -MMD -MP -g
LDLIBS += -lm $(shell pkg-config --libs gtk+-3.0)
LDFLAGS += -rdynamic

# =================
# Compilation rules
# =================
.PHONY: all all_tests
all: $(TARGET)
all_tests: $(TEST_TARGETS)

.PHONY: dist
dist: $(DIST)

$(TARGET): $(TARGET_OBJ) $(OBJS) $(RESOURCES_OBJ)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

$(RESOURCES_SRC): $(RESOURCE_INDEX) $(RESOURCES)
	@mkdir -p $(@D)
	glib-compile-resources --target=$@ --sourcedir=$(RESOURCES_DIR) --generate-source $<

$(RESOURCES_HEADER): $(RESOURCE_INDEX) $(RESOURCES)
	@mkdir -p $(@D)
	glib-compile-resources --target=$@ --sourcedir=$(RESOURCES_DIR) --generate-header $<

$(TEST_BUILD_DIR)/%: LDLIBS += -lcriterion
$(TEST_BUILD_DIR)/%: $(TEST_DIR)/%.c $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_BUILD_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# =================
# Distribution rule
# =================
$(DIST): $(TARGET_SRC) $(SRCS) $(HEADERS) $(RESOURCE_INDEX) $(RESOURCES) $(TEST_SRCS) $(MAKEFILE) $(DOCUMENTATION)
	tar -zcvf $@ $^

# ========================
# Pseudo-target definition
# ========================
.PHONY: test
test: $(TEST_TARGETS)
	for test_file in $^; do ./$$test_file --verbose; done

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
