
# Table of Contents

1.  [Apiofirm - an operating system for IA32-compatible architectures.](#org716480b)
2.  [Features](#org91af678)
3.  [Bootloader](#org4c1db07)
4.  [Building](#orgb7b9aba)


<a id="org716480b"></a>

# Apiofirm - an operating system for IA32-compatible architectures.

Apiofirm is an operating system for IA32-compatible architectures, eventually AMD64, themed around bees.


<a id="org91af678"></a>

# Features

Very few, however:

-   Printing to screen, formatted.
-   Interrupts & IRQS.
-   Functional keyboard handler.
-   Functional keyboard layout.


<a id="org4c1db07"></a>

# Bootloader

Apiofirm boots using the CRXBOOT1 & CRXBOOT bootloaders.
Note: CRXBOOT1 may also be referred to as BOOT.

BOOT supports searching for a Stage-1 Executable File (common extension `.kb`) in the root directory
of a FAT12 filesystem. It is a 512-byte boot sector, stage 1 boot loader.
Specifically, it searches for `crxboot.kb`.

CRXBOOT supports searching for a Stage-2 Executable File (common extension `.k`) in any directory
in a FAT12 filesystem. It is an ELF file and a Stage-1 Executable File, stage 2 boot loader.
Specifically, it searches for `/Boot/system.k`.


<a id="orgb7b9aba"></a>

# Building

Requires `clang` and `nasm`.
Make using `make`.

