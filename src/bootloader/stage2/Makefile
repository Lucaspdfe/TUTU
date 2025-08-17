BUILD?=build

.PHONY: all stage2 clean
all: stage2

stage2: $(BUILD_DIR)/stage2.bin
$(BUILD_DIR)/stage2.bin: main.asm
	@nasm -f bin $< -o $@

clean:
	@rm -f $(BUILD_DIR)/stage2.bin
