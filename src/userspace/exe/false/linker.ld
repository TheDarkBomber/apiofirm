ENTRY(main)
OUTPUT_FORMAT("binary")

SECTIONS {
				 .main	: {__main_start = .; *(.main)}
				 .text	:	{__text_start = .;	*(.text)}
				 .data	:	{__data_start = .;	*(.data)}
				 .rodata	:							{__rodata_start = .;	*(.rodata)}
				 .bss			:							{__bss_start = .;	*(.bss)}

				 __end = .;
}
