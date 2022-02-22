// False
// Userspace executable that returns a non-zero exit code.

int __attribute__((section(".main"))) main() {
	return 1;
}
