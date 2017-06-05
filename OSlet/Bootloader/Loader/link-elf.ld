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
  .data : 
  {
    data = .;
    _data_start = .;
    *(.data)
    _data_end = .;
  }
  lowmem :  
  {
    . = ALIGN(65536); 
    _low_mem_start = .;
    . += 65536;
    _low_mem_end = .;
  }
  .bss : 
  {
    bss = .;
    _bss_start = .;
    *(.bss)
    _bss_end = .;
  }
  end = .;
}
