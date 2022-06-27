BUILD?=build/

CC=clang -target i686-elf
CCFLAGS=-std=c99 -ffreestanding -nostdlib
CCLIBRARIES=-lgcc
LDFLAGS= -T linker.ld -nostdlib
LD=clang -target i686-elf

C_SOURCES=$(wildcard *.c)
C_OBJECTS=$(patsubst %.c, $(OBJ_LOCATION)/%.o, $(C_SOURCES))
OBJ_LOCATION=$(BUILD)/exe/false

.PHONY: all false clean always
all: false

false: $(BUILD)/exe/false.x

$(BUILD)/exe/false.x: $(C_OBJECTS)
	$(LD) $(LDFLAGS) -Wl,-Map=$(BUILD)/exe/false.map -o $@ $^ $(CCLIBRARIES)
	@echo "[FALSE] COMPILED exe/false.x"

$(OBJ_LOCATION)/%.o: %.c always
	mkdir -pv $(@D)
	$(CC) $(CCFLAGS) -c -o $@ $<
	@echo "[FALSE] COMPILED " $<

always:
	mkdir -pv $(OBJ_LOCATION)

clean:
	rm -fv $(BUILD)/exe/false.x
	rm -fv $(BUILD)/exe/false.map
	rm -fv $(OBJ_LOCATION)/*
