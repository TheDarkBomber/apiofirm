BUILD?=build
EFI=$(BUILD)/crxboot.efi

TARGET=$(BUILD)/uefi.img
ESP=$(BUILD)/esp.img

all: crxboot
crxboot: $(ESP)
	dd if=$(ESP) of=$(TARGET) bs=512 count=91669 seek=2048 conv=notrunc

$(EFI): crxboot/boot.c
	$(MAKE) -C crxboot BUILD=$(abspath $(BUILD))

$(TARGET):
	dd if=/dev/zero of=$(TARGET) bs=512 count=93750
	parted $(TARGET) -s -a minimal mklabel gpt
	parted $(TARGET) -s -a minimal mkpart EFI FAT16 2048s 93716s
	parted $(TARGET) -s -a minimal toggle 1 boot

$(ESP): $(TARGET) $(EFI)
	dd if=/dev/zero of=$(ESP) bs=512 count=91669
	mformat -i $(ESP) -h 32 -t 32 -n 64 -c 1
	mmd -i $(ESP) "::EFI"
	mmd -i $(ESP) "::EFI/BOOT"
	mcopy -i $(ESP) $(EFI) "::EFI/BOOT/BOOTX64.efi"
