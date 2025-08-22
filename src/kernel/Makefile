BUILD?=build

.PHONY: all kernel clean
all: kernel

kernel: $(BUILD_DIR)/kernel.bin
$(BUILD_DIR)/kernel.bin: main.asm
	@nasm -f bin $< -o $@

clean:
	@rm -f $(BUILD_DIR)/kernel.bin
