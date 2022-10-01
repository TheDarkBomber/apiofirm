#include <panic.h>
#include <hci/text.h>
#include <colours.h>

void GenericKernelPanic(const char* format, char* error, ...) {
	prints("\x1B[1;31m***FATAL ERROR***\n\x1B[0m");
	va_list arguments;
	va_start(arguments, error);

	TextCTX.Background = SOFTMAGENTA;
	TextCTX.Foreground = WHITE;
	TextCTX.CursorX = 0;
	TextCTX.CursorY = 0;
	ClearScreen(0);

	printf("KERNEL PANIC\n\n%s\n", error);
	printfv(format, arguments);

	asm ("cli; hlt");
	for(;;);
}
