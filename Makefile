ASSEMBLER=nasm

SOURCES=src
BUILD=build

BOOTFILE=$(BUILD)/boot.x86
IMAGEFILE=$(BUILD)/apiofirm.img
KERNEL=$(BUILD)/system.k

image: boot kernel
	dd if=/dev/zero of=$(IMAGEFILE) bs=512 count=2880
	mkfs.fat -F 12 -n "IMAGE" $(IMAGEFILE)
	dd if=$(BOOTFILE) of=$(IMAGEFILE) conv=notrunc
	mcopy -i $(IMAGEFILE) $(KERNEL) "::system.k"

boot: $(SOURCES)/boot.asm
	$(ASSEMBLER) $(SOURCES)/boot.asm -f bin -o $(BOOTFILE)

kernel: $(SOURCES)/kernel.asm
	$(ASSEMBLER) $(SOURCES)/kernel.asm -f bin -o $(KERNEL)

clean:
	rm -v $(BOOTFILE)
	rm -v $(IMAGEFILE)
	rm -v $(KERNEL)

deepclean:
	rm -rv $(BUILD)/*
