// False
// Userspace executable that returns a non-zero exit code.

int __attribute__((section(".main"))) main() {
	__asm__ volatile (".intel_syntax noprefix; jmp $; .att_syntax");
	return 1;
}
