
#define VIDEOMEM 0xa0000

#define INIT_REGISTERS asm( 	"mov $0x10, %eax\n"	\
	    			"mov %ax, %ds\n"		\
	     			"mov %ax, %es\n"		\
	     			"mov %ax, %gs\n"		\
	     			"mov %ax, %fs\n"		\
	     			"mov %ax, %ss\n"		\
	     			"mov $0x9fff0, %esp\n");


