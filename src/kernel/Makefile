BUILD?=build
# SOURCES=kmain.c

CC=clang -target x86_64-elf
LD=ld.lld

C_SOURCES=$(wildcard *.c)
C_OBJECTS=$(patsubst %.c, $(BUILD)/kernel/%.o, $(C_SOURCES))

CCFLAGS=-ffreestanding -fno-stack-protector -fno-stack-check -D__x86_64__ -Wreturn-type
LDFLAGS=-nostdlib -z max-page-size=0x1000 -Ttext=0x01000000 -T linker.ld

all: $(BUILD)/system.k

$(BUILD)/kernel/%.o: %.c always
	mkdir -pv $(@D)
	$(CC) $(CCFLAGS) -c $< -o $@
	@echo "[KERNEL] COMPILED " $<


$(BUILD)/system.k: $(C_OBJECTS)
	mkdir -pv $(@D)
	$(LD) $(LDFLAGS) $^ -o $@
	@echo "[KERNEL] LINKED KERNEL"

always:
	mkdir -pv $(BUILD)/kernel
