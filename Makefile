SRC_DIR=src
BUILD_DIR=build

.PHONY: all clean always bootloader floppy run
all: clean always floppy

floppy: $(BUILD_DIR)/floppy.img
$(BUILD_DIR)/floppy.img: bootloader kernel
	@dd if=/dev/zero of=$@ bs=512 count=2880 >/dev/null 2>&1
	@mkfs.fat -F 12 -n "NBOS" $@ >/dev/null 2>&1
	@dd if=$(BUILD_DIR)/stage1.bin of=$@ conv=notrunc >/dev/null 2>&1
	@mcopy -i $@ $(BUILD_DIR)/stage2.bin "::stage2.bin"
	@mcopy -i $@ $(BUILD_DIR)/kernel.bin "::kernel.bin"
	@mcopy -i $@ test.txt "::test.txt"
	@mmd -i $@ "::mydir"
	@mcopy -i $@ test.txt "::mydir/test.txt"
	@echo "--> Final file: " $@

bootloader: stage1 stage2

stage1: $(BUILD_DIR)/stage1.bin
$(BUILD_DIR)/stage1.bin: always
	@echo "- Building stage1 ($@)"
	@$(MAKE) -C $(SRC_DIR)/bootloader/stage1 BUILD_DIR=$(abspath $(BUILD_DIR)) --no-print-directory

stage2: $(BUILD_DIR)/stage2.bin
$(BUILD_DIR)/stage2.bin: always
	@echo "- Building stage2 ($@)"
	@$(MAKE) -C $(SRC_DIR)/bootloader/stage2 BUILD_DIR=$(abspath $(BUILD_DIR)) --no-print-directory

kernel: $(BUILD_DIR)/kernel.bin
$(BUILD_DIR)/kernel.bin: always
	@echo "- Building kernel ($@)"
	@$(MAKE) -C $(SRC_DIR)/kernel BUILD_DIR=$(abspath $(BUILD_DIR)) --no-print-directory

always:
	@mkdir -p $(BUILD_DIR)

clean:
	@$(MAKE) -C $(SRC_DIR)/bootloader/stage1 clean BUILD_DIR=$(abspath $(BUILD_DIR)) --no-print-directory
	@$(MAKE) -C $(SRC_DIR)/bootloader/stage2 clean BUILD_DIR=$(abspath $(BUILD_DIR)) --no-print-directory
	@$(MAKE) -C $(SRC_DIR)/kernel clean BUILD_DIR=$(abspath $(BUILD_DIR)) --no-print-directory

run: all
	@echo "--> Running the system..."
	@qemu-system-x86_64 -drive format=raw,file=build/floppy.img,if=floppy
