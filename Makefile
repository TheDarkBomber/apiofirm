BUILD=build
SOURCES=src
RESOURCES=res

UEFI=$(BUILD)/uefi.img
ESP=$(BUILD)/esp.img

EXTLIB=$(abspath extlib)
LAI=$(abspath lightweight-aml-interpreter)

all: kernel image
image: $(UEFI) $(ESP)
	dd if=$(ESP) of=$(UEFI) bs=512 count=91669 seek=2048 conv=notrunc

kernel: $(BUILD)/system.k $(ESP)
	mmd -i $(ESP) "::apiofirm"
	mmd -i $(ESP) "::apiofirm/bin"
	mmd -i $(ESP) "::apiofirm/res"
	mcopy -i $(ESP) $(BUILD)/system.k "::apiofirm/bin/system.k"
	mcopy -i $(ESP) $(RESOURCES)/uegetf-8col.psf "::apiofirm/res/confon.psf"
	mcopy -i $(ESP) $(RESOURCES)/apiofirm.txt "::apiofirm/apiofirm.txt"

$(BUILD)/system.k:
	$(MAKE) -C $(SOURCES)/kernel BUILD=$(abspath $(BUILD)) EXTLIB=$(abspath $(EXTLIB))

$(UEFI): $(ESP)
$(ESP):
	$(MAKE) -C $(SOURCES)/boot BUILD=$(abspath $(BUILD))

lai:
	cd $(LAI) && \
	mkdir -v $(LAI)/build && \
	cd $(LAI)/build && \
	meson .. && \
	ninja && \
	mv -v liblai.a $(EXTLIB) && \
	cp -rv liblai.a.p $(EXTLIB) && \
	rm -r $(LAI)/build

clean:
	rm -rfv $(BUILD)/* > /dev/null
