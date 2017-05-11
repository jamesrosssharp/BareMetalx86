OUTPUT_FORMAT("binary")
OUTPUT_ARCH("i386")
ENTRY(start)
phys = 0x00020000;
SECTIONS
{
  .text phys : AT(phys) {
    code = .;
    *(.text)
    *(.rodata)
    . = ALIGN(4096);
  }
  .data : AT(phys + (data - code))
  {
    data = .;
    *(.data)
    . = ALIGN(65536); /* align to a segment so that the BSS starts on a segment
		      # aligned address, and hopefully doesn't cross segment
		      # boundaries, because we will use real mode accesses at 
		      # on bss data (for now)... TODO: clean this up so that
		      # we use a section of memory at a real mode address
		      # for a special allocator to allocate real-mode 
		      # accessible memory as necessary. */
  }
  .bss : AT(phys + (bss - code))
  {
    bss = .;
    *(.bss)
    . = ALIGN(65536);
  }
  end = .;
}
