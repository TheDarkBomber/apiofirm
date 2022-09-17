BUILD=build
SOURCES=src
RESOURCES=res

UEFI=$(BUILD)/uefi.img
ESP=$(BUILD)/esp.img

all: kernel image
image: $(UEFI) $(ESP)
	dd if=$(ESP) of=$(UEFI) bs=512 count=91669 seek=2048 conv=notrunc

kernel: $(BUILD)/system.k $(ESP)
	mcopy -i $(ESP) $(BUILD)/system.k "::system.k"
	mcopy -i $(ESP) $(RESOURCES)/uegetf-8col.psf "::confon.psf"

$(BUILD)/system.k:
	$(MAKE) -C $(SOURCES)/kernel BUILD=$(abspath $(BUILD))

$(UEFI): $(ESP)
$(ESP):
	$(MAKE) -C $(SOURCES)/boot BUILD=$(abspath $(BUILD))

clean:
	rm -rfv $(BUILD)/* > /dev/null
