#include "keyboard.h"
#include "x86.h"
#include <stdint.h>
#include <stdbool.h>

static bool MagicKeyActive = false;
static bool ExtendedMode = false;

void handleMagic() {
	MagicKeyActive = !MagicKeyActive;
	if (MagicKeyActive) __asm__ volatile ("int $0x03"); // Breakpoint.
}

void keyboardHandler(uint8_t interrupt) {
  PICSendEndOfInterrupt(interrupt);
  uint8_t KeyboardStatus = x86Input(0x64);
  if (KeyboardStatus & 1) {
    uint8_t KeyboardScancode = x86Input(0x60);
    print("A key was pressed. Scancode is %u.\r\n", KeyboardScancode);
		if (KeyboardScancode == KEY_MAGIC) handleMagic();
		if (!ExtendedMode && KeyboardScancode == KEY_EXTENDED) {
			ExtendedMode = true;
			return;
		}
		ExtendedMode = false;
  }
}
