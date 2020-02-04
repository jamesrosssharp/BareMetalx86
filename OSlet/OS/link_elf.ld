OUTPUT_ARCH("i386")
ENTRY(start)
phys = 0x01000000;
SECTIONS
{
  . = phys;

  .startup : {
	__boot_section__ = .;
	*(.startup)
	. = ALIGN(4096); /* Align to page boundary */
  }

  .text : {
    code = .;
    *(.text)
    *(.rodata)
    . = ALIGN(4096);
  }
  .data :
  {
    data = .;
    *(.data)
    . = ALIGN(4096);
  }
  .bss :
  {
    bss = .;
    *(.bss)
    . = ALIGN(4096);
  }
  end = .;
}
