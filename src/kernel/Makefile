BUILD_DIR?=build/
ASM?=nasm

.PHONY: all clean
all: kernel 

kernel: $(BUILD_DIR)/kernel.bin
$(BUILD_DIR)/kernel.bin: main.asm
	$(ASM) $< -f bin -o $@

clean:
	rm -f $(BUILD_DIR)/kernel.bin