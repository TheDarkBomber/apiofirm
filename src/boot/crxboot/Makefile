BUILD?=build/
ASSEMBLER?=nasm
ASSEMBLER_FLAGS?=-f elf
WATCOM?=/usr/bin/watcom
CC16?=$(WATCOM)/binl/wcc
CCFLAGS16?=-4 -d3 -s -wx -ms -zl -za99
LD16?=$(WATCOM)/binl/wlink

CC=clang -target i686-elf
CCFLAGS=-std=c99 -ffreestanding -nostdlib
CCLIBRARIES=-lgcc
LDFLAGS= -T linker.ld -nostdlib
LD=clang -target i686-elf

C_SOURCES=$(wildcard *.c)
ASM_SOURCES=$(wildcard *.asm)
C_OBJECTS=$(patsubst %.c, $(OBJ_LOCATION)/%.o, $(C_SOURCES))
ASM_OBJECTS=$(patsubst %.asm, $(OBJ_LOCATION)/%.x86, $(ASM_SOURCES))
OBJ_LOCATION=$(BUILD)/boot/crxboot

.PHONY: all crxboot clean always
all: crxboot

crxboot: $(BUILD)/crxboot.kb

$(BUILD)/crxboot.kb: $(ASM_OBJECTS) $(C_OBJECTS)
	$(LD) $(LDFLAGS) -Wl,-Map=$(BUILD)/crxboot.map -o $@ $^ $(CCLIBRARIES)
	@echo "[CRXBOOT] COMPILED crxboot.kb"

$(OBJ_LOCATION)/%.o: %.c always
	mkdir -pv $(@D)
	$(CC) $(CCFLAGS) -c -o $@ $<
	@echo "[CRXBOOT] COMPILED " $<

$(OBJ_LOCATION)/%.x86: %.asm always
	mkdir -pv $(@D)
	$(ASSEMBLER) $(ASSEMBLER_FLAGS) -o $@ $<
	@echo "[CRXBOOT] COMPILED " $<

always:
	mkdir -pv $(OBJ_LOCATION)

clean:
	rm -fv $(BUILD)/crxboot.kb
	rm -fv $(BUILD)/crxboot.map
	rm -fv $(OBJ_LOCATION)/*
