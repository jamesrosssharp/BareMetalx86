/*

	Fixed size K&R memory allocator
	(no provision for heap to grow)
						*/

#pragma once

struct KRBlockHeader
{
	struct KRBlockHeader* next;
	unsigned int size; // size in units of header
};


struct KRMemoryAllocator 
{

	struct KRBlockHeader* head;

	unsigned int size;	// size in bytes of memory available for allocation
	unsigned int units;

	char data[]; // struct hack... point to start of data memory		
};

#ifdef __cplusplus
extern "C" {
#endif

bool		mem_kr_init(void* mem, unsigned int memSize);
void*		mem_kr_allocate(struct KRMemoryAllocator* kr, unsigned int bytes);
void 		mem_kr_debug(struct KRMemoryAllocator* kr);
void		mem_kr_free(struct KRMemoryAllocator* kr, void* address);
unsigned int 	mem_kr_createMemoryPool(struct MemoryPool* pool, void* dataStart, unsigned int size);

#ifdef __cplusplus
}
#endif
