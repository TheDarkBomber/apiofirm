ASSEMBLER=nasm
SOURCES=src
BUILD=build
TOOLCHAIN=toolchain

BOOTFILE=$(BUILD)/boot.x86
IMAGEFILE=$(BUILD)/apiofirm.img
CRXBOOT=$(BUILD)/crxboot.kb
KERNEL=$(BUILD)/system.k

all: image

image: boot kernel crxboot false
	dd if=/dev/zero of=$(IMAGEFILE) bs=512 count=2880
	mkfs.fat -F 12 -n "APIOFIRMCRX" $(IMAGEFILE)
	dd if=$(BOOTFILE) of=$(IMAGEFILE) conv=notrunc
	mmd -i $(IMAGEFILE) "::boot"
	mcopy -i $(IMAGEFILE) $(KERNEL) "::boot/system.k"
	mcopy -i $(IMAGEFILE) $(CRXBOOT) "::crxboot.kb"
	mmd -i $(IMAGEFILE) "::bees"
	mcopy -i $(IMAGEFILE) apioform.bee "::bees/apioform.bee"
	mmd -i $(IMAGEFILE) "::exe"
	mcopy -i $(IMAGEFILE) $(BUILD)/exe/false.x "::exe/false.x"

boot: $(SOURCES)/boot/boot.asm always
	$(ASSEMBLER) $(SOURCES)/boot/boot.asm -f bin -o $(BOOTFILE)

kernel: $(KERNEL)

crxboot: $(CRXBOOT)

$(CRXBOOT): always
	$(MAKE) -C $(SOURCES)/boot/crxboot BUILD=$(abspath $(BUILD))

$(KERNEL): always
	$(MAKE) -C $(SOURCES)/kernel BUILD=$(abspath $(BUILD))

false: $(BUILD)/exe/false.x

$(BUILD)/exe/false.x: always
	$(MAKE) -C $(SOURCES)/userspace/exe/false BUILD=$(abspath $(BUILD))

always:
	mkdir -pv $(BUILD)

clean:
	rm -v $(BOOTFILE)
	rm -v $(IMAGEFILE)
	rm -v $(KERNEL)
	$(MAKE) -C $(SOURCES)/boot/crxboot BUILD=$(abspath $(BUILD)) clean
	$(MAKE) -C $(SOURCES)/kernel BUILD=$(abspath $(BUILD)) clean

deepclean:
	rm -rv $(BUILD)/*
