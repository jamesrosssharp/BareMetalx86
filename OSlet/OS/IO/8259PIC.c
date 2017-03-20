
#include "8259PIC.h"

#define PIC_MASTER_PORT_CMD	0x20
#define PIC_MASTER_PORT_DATA	0x21

#define PIC_SLAVE_PORT_CMD	0xa0
#define PIC_SLAVE_PORT_DATA	0xa1

#define PIC_ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define PIC_ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define PIC_ICW1_INTERVAL4 0x04		/* Call address interval 4 (8) */
#define PIC_ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define PIC_ICW1_INIT	0x10		/* Initialization - required! */
 
#define PIC_ICW4_8086		0x01	/* 8086/88 (MCS-80/85) mode */
#define PIC_ICW4_AUTO		0x02	/* Auto (normal) EOI */
#define PIC_ICW4_BUF_SLAVE	0x08	/* Buffered mode/slave */
#define PIC_ICW4_BUF_MASTER	0x0C	/* Buffered mode/master */
#define PIC_ICW4_SFNM		0x10	/* Special fully nested (not) */

#define PIC_MASTER_PM_VECTOR_OFFSET	0x20
#define PIC_SLAVE_PM_VECTOR_OFFSET	0x28

#define PIC_MASTER_RM_VECTOR_OFFSET	0x08
#define PIC_SLAVE_RM_VECTOR_OFFSET	0x70

unsigned char gRealModeMasterPICInterruptMask = 0;
unsigned char gRealModeSlavePICInterruptMask = 0;

unsigned char gProtectedModeMasterPICInterruptMask = 0;
unsigned char gProtectedModeSlavePICInterruptMask = 0;

static	void	io_smallDelay();
static  void	io_remapPICs(unsigned char masterVectorOffset, unsigned char slaveVectorOffset);

bool	io_initAndRemapPIC()
{
	// preserve previous (real mode) interrupt mask

	gRealModeMasterPICInterruptMask = inByte(PIC_MASTER_PORT_DATA);
	gRealModeSlavePICInterruptMask = inByte(PIC_SLAVE_PORT_DATA);

	kprintf("Saving old PIC mask state: %02x %02x\n", 
		gRealModeMasterPICInterruptMask,
		gRealModeSlavePICInterruptMask 
	);
	
	io_remapPICs(PIC_MASTER_PM_VECTOR_OFFSET, PIC_SLAVE_PM_VECTOR_OFFSET);

	// restore saved interrupt masks

	outByte(PIC_MASTER_PORT_DATA, gRealModeMasterPICInterruptMask);
	outByte(PIC_SLAVE_PORT_DATA,  gRealModeSlavePICInterruptMask);

}

bool	io_restorePICRealModeConfiguration()
{

	// Init pic and restore to real mode vectors

	io_remapPICs(PIC_MASTER_RM_VECTOR_OFFSET, PIC_SLAVE_RM_VECTOR_OFFSET);

	// Restore masks

	outByte(PIC_MASTER_PORT_DATA, gRealModeMasterPICInterruptMask);
	outByte(PIC_SLAVE_PORT_DATA,  gRealModeSlavePICInterruptMask);

}

void	io_smallDelay()
{
	for (volatile unsigned int i = 0; i < 0x0000ffff; i ++)
		asm ("nop\n" : : :);
}


void	io_remapPICs(unsigned char masterVectorOffset, unsigned char slaveVectorOffset)
{
	// Init PIC and remap vector offsets 

	kprintf("Remapping PICs...\n");

	outByte(PIC_MASTER_PORT_CMD, PIC_ICW1_INIT | PIC_ICW1_ICW4);
	io_smallDelay();

	outByte(PIC_SLAVE_PORT_CMD, PIC_ICW1_INIT | PIC_ICW1_ICW4);
	io_smallDelay();

	outByte(PIC_MASTER_PORT_DATA, masterVectorOffset);
	io_smallDelay();

	outByte(PIC_SLAVE_PORT_DATA, slaveVectorOffset);
	io_smallDelay();

	outByte(PIC_MASTER_PORT_DATA, 4);	// Master has a slave
	io_smallDelay();
	
	outByte(PIC_SLAVE_PORT_DATA, 2);	// Cascade the slave
	io_smallDelay();

	outByte(PIC_MASTER_PORT_DATA, PIC_ICW4_8086);
	io_smallDelay();

	outByte(PIC_SLAVE_PORT_DATA, PIC_ICW4_8086);
	io_smallDelay();	

}
