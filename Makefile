ASSEMBLER=nasm

SOURCES=src
BUILD=build

BOOTFILE=$(BUILD)/boot.x86
IMAGEFILE=$(BUILD)/apiofirm.img

image: boot
	cp -v $(BOOTFILE) $(IMAGEFILE)
	truncate -s 1440k $(IMAGEFILE)

boot: $(SOURCES)/boot.asm
	$(ASSEMBLER) $(SOURCES)/boot.asm -f bin -o $(BOOTFILE)

clean:
	rm -v $(BOOTFILE)
	rm -v $(IMAGEFILE)

deepclean:
	rm -rv $(BUILD)/*
