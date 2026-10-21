
CC = gcc
CFLAGS  = -std=c99
CFLAGS += -g
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -pedantic
#CFLAGS += -Wmissing-declarations
CFLAGS += -O2

LDLIBS ?= -lgsl -lgslcblas -lm

TARGET_EXEC ?= run_feat

BUILD_DIR ?= ./bin
SRC_DIRS ?= ./src

SRCS := $(shell find $(SRC_DIRS) -name "*.c")
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(CFLAGS) $(LDLIBS)

# Value checks against hctsa v1.03: `make test`
TEST_EXEC ?= run_tests
LIB_OBJS := $(filter-out %/main.c.o,$(OBJS))

$(TEST_EXEC): $(LIB_OBJS) $(BUILD_DIR)/tests/test_features.c.o
	$(CC) $^ -o $@ $(LDFLAGS) $(CFLAGS) $(LDLIBS)

test: $(TEST_EXEC)
	./$(TEST_EXEC) tests/data ../test_ts.csv

$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) -c $< -o $@ $(CFLAGS)

.PHONY: clean test

clean:
	$(RM) -r $(BUILD_DIR) $(TEST_EXEC)

-include $(DEPS)

MKDIR_P ?= mkdir -p
