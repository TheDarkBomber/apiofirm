ASSEMBLER=nasm
SOURCES=src
BUILD=build

BOOTFILE=$(BUILD)/boot.x86
IMAGEFILE=$(BUILD)/apiofirm.img
CRXBOOT=$(BUILD)/crxboot.kb
KERNEL=$(BUILD)/system.k

image: boot kernel crxboot
	dd if=/dev/zero of=$(IMAGEFILE) bs=512 count=2880
	mkfs.fat -F 12 -n "APIOFIRMCRX" $(IMAGEFILE)
	dd if=$(BOOTFILE) of=$(IMAGEFILE) conv=notrunc
	mcopy -i $(IMAGEFILE) $(KERNEL) "::system.k"
	mcopy -i $(IMAGEFILE) $(CRXBOOT) "::crxboot.kb"

boot: $(SOURCES)/boot/boot.asm always
	$(ASSEMBLER) $(SOURCES)/boot/boot.asm -f bin -o $(BOOTFILE)

kernel: $(SOURCES)/kernel.asm always
	$(ASSEMBLER) $(SOURCES)/kernel.asm -f bin -o $(KERNEL)

crxboot: $(CRXBOOT)

$(CRXBOOT): always
	$(MAKE) -C $(SOURCES)/boot/crxboot BUILD=$(abspath $(BUILD))

always:
	mkdir -pv $(BUILD)

clean:
	rm -v $(BOOTFILE)
	rm -v $(IMAGEFILE)
	rm -v $(KERNEL)
	$(MAKE) -C $(SOURCES)/boot/crxboot BUILD=$(abspath $(BUILD)) clean

deepclean:
	rm -rv $(BUILD)/*
