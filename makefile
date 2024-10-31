CC = gcc
CFLAGS = -Wall -Wextra -g -MMD -MP

TARGET_EXEC = flyingfish

BUILD_DIR = ./build
SRC_DIRS = ./src

SRCS = $(shell find $(SRC_DIRS) -name '*.c')
OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

INC_DIRS = $(shell find $(SRC_DIRS) -type d)
INC_FLAGS = $(addprefix -I,$(INC_DIRS))

# Final executable
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Build object files and dependency files
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC_FLAGS) -c $< -o $@

# Include dependency files
-include $(DEPS)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
