
# Table of Contents

1.  [Apiofirm - an operating system for IA32-compatible architectures.](#org6a5368e)
2.  [Features](#org9d11d19)
3.  [Bootloader](#org3d4a9b0)
4.  [Building](#org8223bb9)


<a id="org6a5368e"></a>

# Apiofirm - an operating system for IA32-compatible architectures.

Apiofirm is an operating system for IA32-compatible architectures, eventually AMD64, themed around bees.


<a id="org9d11d19"></a>

# Features

Very few, however:

-   Printing to screen, formatted.
-   Interrupts & IRQS.
-   Functional keyboard handler.
-   Functional keyboard layout.


<a id="org3d4a9b0"></a>

# Bootloader

Apiofirm boots using the CRXBOOT1 & CRXBOOT bootloaders.
Note: CRXBOOT1 may also be referred to as BOOT.

BOOT supports searching for a Stage-1 Executable File (common extension `.kb`) in the root directory
of a FAT12 filesystem. It is a 512-byte boot sector, stage 1 boot loader.
Specifically, it searches for `crxboot.kb`.

CRXBOOT supports searching for a Stage-2 Executable File (common extension `.k`) in any directory
in a FAT12 filesystem. It is an ELF file and a Stage-1 Executable File, stage 2 boot loader.
Specifically, it searches for `/Boot/system.k`.


<a id="org8223bb9"></a>

# Building

Requires an[ i686 Cross Compiler](https://wiki.osdev.org/GCC_Cross-Compiler) with a system root in the `toolchain/` directory of the project root.
To build apiofirm, simply run `make` in the project root. This will build BOOT, CRXBOOT, the kernel,
and a floppy image which stores these files.

Alternatively, you may manually build each file, or run `make` in the project root for each target separately,
or just `make image` since that makes all of them, at the moment, or do whatever.

