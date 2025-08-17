SRC_DIR=src
BUILD_DIR=build

.PHONY: all clean always bootloader floppy run
all: clean always floppy

floppy: $(BUILD_DIR)/floppy.img
$(BUILD_DIR)/floppy.img: bootloader kernel always
	@echo " - Building floppy image ($@)"
	@dd if=/dev/zero of=$(BUILD_DIR)/floppy.img bs=512 count=2880
	@mkfs.fat -F 12 -n "TUTU OS" $(BUILD_DIR)/floppy.img
	@dd if=$(BUILD_DIR)/boot.bin of=$(BUILD_DIR)/floppy.img bs=512 count=1 conv=notrunc
	@mcopy -i $(BUILD_DIR)/floppy.img $(BUILD_DIR)/kernel.bin ::kernel.bin

bootloader: $(BUILD_DIR)/boot.bin
$(BUILD_DIR)/boot.bin: always
	@echo " - Building bootloader ($@)"
	@nasm -f bin $(SRC_DIR)/bootloader/boot.asm -o $(BUILD_DIR)/boot.bin

kernel: $(BUILD_DIR)/kernel.bin
$(BUILD_DIR)/kernel.bin: always
	@echo " - Building kernel ($@)"
	@nasm -f bin $(SRC_DIR)/kernel/main.asm -o $(BUILD_DIR)/kernel.bin

always:
	@mkdir -p $(BUILD_DIR)

clean:
	@rm -rf $(BUILD_DIR)

run: all
	@qemu-system-x86_64 -fda $(BUILD_DIR)/floppy.img
