BUILD_DIR?=build/
ASM?=nasm

.PHONY: all clean
all: stage2 

stage2: $(BUILD_DIR)/stage2.bin
$(BUILD_DIR)/stage2.bin: main.asm
	$(ASM) $< -f bin -o $@

clean:
	rm -f $(BUILD_DIR)/stage2.bin