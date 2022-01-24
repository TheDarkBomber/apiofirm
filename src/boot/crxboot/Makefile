BUILD?=build/
ASSEMBLER?=nasm
ASSEMBLER_FLAGS?=-f obj
WATCOM?=/usr/bin/watcom
CC16?=$(WATCOM)/binl/wcc
CCFLAGS16?=-4 -d3 -s -wx -ms -zl -za99
LD16?=$(WATCOM)/binl/wlink

C_SOURCES=$(wildcard *.c)
ASM_SOURCES=$(wildcard *.asm)
C_OBJECTS=$(patsubst %.c, $(OBJ_LOCATION)/%.o, $(C_SOURCES))
ASM_OBJECTS=$(patsubst %.asm, $(OBJ_LOCATION)/%.x86, $(ASM_SOURCES))
OBJ_LOCATION=$(BUILD)/boot/crxboot

.PHONY: all crxboot clean always
all: crxboot

crxboot: $(BUILD)/crxboot.kb

$(BUILD)/crxboot.kb: $(ASM_OBJECTS) $(C_OBJECTS)
	$(LD16) NAME $(BUILD)/crxboot.kb FILE \{ $(ASM_OBJECTS) $(C_OBJECTS) \} OPTION MAP=$(BUILD)/crxboot.map @linker.ld

$(OBJ_LOCATION)/%.o: %.c always
	$(CC16) $(CCFLAGS16) -fo=$@ $<

$(OBJ_LOCATION)/%.x86: %.asm always
	$(ASSEMBLER) $(ASSEMBLER_FLAGS) -o $@ $<

always:
	mkdir -pv $(OBJ_LOCATION)

clean:
	rm -fv $(BUILD)/crxboot.kb
	rm -fv $(BUILD)/crxboot.map
	rm -fv $(OBJ_LOCATION)/*
