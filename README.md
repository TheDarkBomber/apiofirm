
# Table of Contents

1.  [Apiofirm - an operating system for IA32-compatible architectures.](#orgb2dffef)
2.  [Features](#orgada0f2e)
3.  [Bootloader](#org4465563)
4.  [Building](#org22daa64)
    1.  [i686 Cross Compiler modifications](#org044e72b)


<a id="orgb2dffef"></a>

# Apiofirm - an operating system for IA32-compatible architectures.

Apiofirm is an operating system for IA32-compatible architectures, eventually AMD64, themed around bees.


<a id="orgada0f2e"></a>

# Features

Very few, however:

-   Printing to screen, formatted.
-   Interrupts & IRQS.
-   Functional keyboard handler.
-   Functional keyboard layout.


<a id="org4465563"></a>

# Bootloader

Apiofirm boots using the CRXBOOT1 & CRXBOOT bootloaders.
Note: CRXBOOT1 may also be referred to as BOOT.

BOOT supports searching for a Stage-1 Executable File (common extension `.kb`) in the root directory
of a FAT12 filesystem. It is a 512-byte boot sector, stage 1 boot loader.
Specifically, it searches for `crxboot.kb`.

CRXBOOT supports searching for a Stage-2 Executable File (common extension `.k`) in any directory
in a FAT12 filesystem. It is an ELF file and a Stage-1 Executable File, stage 2 boot loader.
Specifically, it searches for `/Boot/system.k`.


<a id="org22daa64"></a>

# Building

Requires an [i686 Cross Compiler](https://wiki.osdev.org/GCC_Cross-Compiler) with a system root in the `toolchain/` directory of the project root.
To build apiofirm, simply run `make` in the project root. This will build BOOT, CRXBOOT, the kernel,
and a floppy image which stores these files.

Alternatively, you may manually build each file, or run `make` in the project root for each target separately,
or just `make image` since that makes all of them, at the moment, or do whatever.


<a id="org044e72b"></a>

## i686 Cross Compiler modifications

In order for Apiofirm to build correctly, a standard include file, located in `toolchain/lib/gcc/i686-elf/11.1.0/include`
named `stddef.h` needs to be modified.
Specifically, you will need to replace definitions for uppercase NULL with lowercase null. Apiofirm uses lowercase null for consistency
with true and false. This can be done by navigating to the aforesaid directory and executing the command `sed -i 's/#define NULL/#define null/g' stddef.h`.

