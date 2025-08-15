SRC_DIR=src
BUILD_DIR=build

.PHONY: all clean always bootloader floppy run
all: clean always floppy

floppy: bootloader always
	@echo " - Building floppy image ($(BUILD_DIR)/floppy.img)"
	@cp $(BUILD_DIR)/boot.bin $(BUILD_DIR)/floppy.img

bootloader: $(SRC_DIR)/boot.asm always
	@echo " - Building bootloader ($(BUILD_DIR)/boot.bin)"
	@nasm -f bin $< -o $(BUILD_DIR)/boot.bin

always:
	@mkdir build

clean:
	@rm -rf $(BUILD_DIR)

run: all
	@qemu-system-x86_64 -fda $(BUILD_DIR)/floppy.img
