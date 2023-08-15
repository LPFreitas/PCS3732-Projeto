/* kernel.ld */
SECTIONS {
  . = 0;
  .reset : { *(.reset) }

  .text 0x40000000 : {
    *(.text)
  }
	
  .data 0x4000f000 : {
    *(.data)
  }

  . = . + 4096;
  . = ALIGN(8);
  inicio_stack = .;
}
