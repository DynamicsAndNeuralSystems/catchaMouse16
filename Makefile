
CC = gcc
CFLAGS  = -std=c99
CFLAGS += -g
#CFLAGS += -Wall
#CFLAGS += -Wextra
CFLAGS += -pedantic
#CFLAGS += -Wmissing-declarations
CFLAGS += -w #disable warnings
CFLAGS += -lm -lgsl -lgslcblas #gsl

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
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
