BUILD?=build
EXTLIB?=extlib
# SOURCES=kmain.c

ASM=nasm
CC=clang -target x86_64-elf
LD=ld.lld

C_SOURCES=$(wildcard *.c)
C_OBJECTS=$(patsubst %.c, $(BUILD)/kernel/%.o, $(C_SOURCES))

ASM_SOURCES=$(wildcard *.asm)
ASM_OBJECTS=$(patsubst %.asm, $(BUILD)/kernel/%.x86, $(ASM_SOURCES))

ASMFLAGS=-f elf64
CCFLAGS=-ffreestanding -fno-stack-protector -fno-stack-check -D__x86_64__ -Wreturn-type -I../ext.lai
LDFLAGS=-nostdlib -z max-page-size=0x1000 -Ttext=0x01000000 -T linker.ld

KLIBRARIES=$(EXTLIB)/liblai.a

all: $(BUILD)/system.k

$(BUILD)/kernel/%.o: %.c always
	mkdir -pv $(@D)
	$(CC) $(CCFLAGS) -c $< -o $@
	@echo "[KERNEL] COMPILED " $<

$(BUILD)/kernel/%.x86: %.asm
	mkdir -pv $(@D)
	$(ASM) $(ASMFLAGS) $< -o $@
	@echo "[KERNEL] COMPILED " $<

$(BUILD)/system.k: $(C_OBJECTS) $(ASM_OBJECTS)
	mkdir -pv $(@D)
	$(LD) $(LDFLAGS) $(KLIBRARIES) $^ -o $@
	@echo "[KERNEL] LINKED KERNEL"

always:
	mkdir -pv $(BUILD)/kernel
