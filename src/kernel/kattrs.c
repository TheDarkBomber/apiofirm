#include <kattrs.h>
#include <int/pic.h>

KernelAttributes ApiofirmCTX;

void ApiofirmDisableEOI() {	ApiofirmCTX.EOI = 0; }
void ApiofirmEnableEOI() {
	ApiofirmCTX.EOI = 1;
	PICSendEndOfInterrupt(PICGetISR());
}
