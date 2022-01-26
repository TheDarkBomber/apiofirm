BUILD?=build/
ASSEMBLER?=nasm
ASSEMBLER_FLAGS?=-f elf

CC=i686-elf-gcc
CCFLAGS=-std=c99 -ffreestanding -nostdlib
CCLIBRARIES=-lgcc
LDFLAGS= -T linker.ld -nostdlib
LD=i686-elf-gcc

C_SOURCES=$(wildcard *.c)
ASM_SOURCES=$(wildcard *.asm)
C_OBJECTS=$(patsubst %.c, $(OBJ_LOCATION)/%.o, $(C_SOURCES))
ASM_OBJECTS=$(patsubst %.asm, $(OBJ_LOCATION)/%.x86, $(ASM_SOURCES))
OBJ_LOCATION=$(BUILD)/kernel

.PHONY: all kernel clean always
all: kernel

kernel: $(BUILD)/system.k

$(BUILD)/system.k: $(ASM_OBJECTS) $(C_OBJECTS)
	$(LD) $(LDFLAGS) -Wl,-Map=$(BUILD)/kernel.map -o $@ $^ $(CCLIBRARIES)
	@echo "[KERNEL] COMPILED system.k"

$(OBJ_LOCATION)/%.o: %.c always
	mkdir -pv $(@D)
	$(CC) $(CCFLAGS) -c -o $@ $<
	@echo "[KERNEL] COMPILED " $<

$(OBJ_LOCATION)/%.x86: %.asm always
	mkdir -pv $(@D)
	$(ASSEMBLER) $(ASSEMBLER_FLAGS) -o $@ $<
	@echo "[KERNEL] COMPILED " $<

always:
	mkdir -pv $(OBJ_LOCATION)

clean:
	rm -fv $(BUILD)/system.k
	rm -fv $(BUILD)/kernel.map
	rm -fv $(OBJ_LOCATION)/*
