BUILD_DIR?=build/
CC=gcc

.PHONY: all clean
all: tools_fat

tools_fat: $(BUILD_DIR)/tools/fat
$(BUILD_DIR)/tools/fat:
	mkdir -p $(BUILD_DIR)/tools
	$(CC) -g -o $@ fat.c

clean:
	rm -rf $(BUILD_DIR)/tools