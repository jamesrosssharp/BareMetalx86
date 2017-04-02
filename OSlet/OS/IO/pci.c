
#include "pci.h"

#define PCI_REG_CONFIG_ADDRESS 0xcf8
#define PCI_REG_CONFIG_DATA    0xcfc

#define MAX_BUSES		 256
#define MAX_DEVICES_PER_BUS	 32
#define MAX_FUNCTIONS_PER_DEVICE 8

bool io_probePCIDevice(int bus, int device);
unsigned int io_readPCIConfigurationWord(int bus, int slot, 
					 int function, int offset);


bool io_probePCIBus()
{
	for (int i = 0; i < MAX_BUSES; i++)
	{
		for (int j = 0; j < MAX_DEVICES_PER_BUS; j++)
		{
			io_probePCIDevice(i, j);	
		}
	}
}


bool io_probePCIDevice(int bus, int device)
{
	for (int function = 0; function < 8; function ++)
	{

		int deviceAndVendorID = 
			io_readPCIConfigurationWord(bus, device, function, 0);

		if (deviceAndVendorID == 0xffffffff)	// Device does not exist. Continue (there may be other functions)
			continue;

		kprintf("Device found: (%02x:%1x:%1x) %04x : %04x \n", 
				bus, device, function,	
				deviceAndVendorID & 0x0000ffff,
				(deviceAndVendorID & 0xffff0000) >> 16);

		unsigned int classSubClass = 
			io_readPCIConfigurationWord(bus, device, function, 8);

		kprintf("    class: %02x subclass: %02x Prog IF: %02x Rev ID: %02x\n",
				(classSubClass & 0xff000000) >> 24,
				(classSubClass & 0x00ff0000) >> 16,
				(classSubClass & 0x0000ff00) >> 8,
				(classSubClass & 0x000000ff));

		unsigned int header = 
			io_readPCIConfigurationWord(bus, device, function, 0x0c);
		
		kprintf("    BIST: %02x header: %02x Latency timer: %02x Cache line size: %02x\n",
				(header & 0xff000000) >> 24,
				(header & 0x00ff0000) >> 16,
				(header & 0x0000ff00) >> 8,
				(header & 0x000000ff));

		// print out BARs

		for (int bar = 0; bar < 5; bar ++)
		{	

			int offset = bar*4 + 0x10;

			unsigned int barData = io_readPCIConfigurationWord(bus, device, function, offset);

			if ((barData & 1) == 0)
			{
				// Memory BAR

				int type = barData & 0x6 >> 1;

				if (type == 0)
				{
					//kprintf("   %d: Memory BAR (32 bit): %08x\n", bar, barData & 0xfffffff0);
				}
				else if (type == 2)
				{
					bar ++;
					offset += 4;
					unsigned int barData2 = io_readPCIConfigurationWord(bus, device, function, offset);
 
					//kprintf("   %d: Memory BAR (64 bit): %08x%08x\n", bar, barData2, barData & 0xfffffff0);
				}
				else
				{
					// Reserved BAR
					//kprintf("   %d: Reserved BAR (32 bit): %08x\n", bar, barData & 0xfffffff0);
				}
	
			} 
			else
			{
				// IO Bar

				
				//kprintf("   %d: IO BAR: %08x\n", bar, barData & 0xfffffffc);

			}

		}

		unsigned char headerByte = (header & 0x00ff0000) >> 16;

		if (! headerByte & 0x80)
			break;

		// must  be multifunction. Loop again.

	}

	return true;
}

unsigned int io_readPCIConfigurationWord(int bus, int slot, 
					 int function, int offset)
{

	unsigned int address = 0x80000000 | ((bus & 0xff) << 16) |
				((slot & 0x1f) << 11) |
				((function & 0x07) << 8) | (offset & 0xfc);

	outLong(PCI_REG_CONFIG_ADDRESS, address);

	return inLong(PCI_REG_CONFIG_DATA);

}
