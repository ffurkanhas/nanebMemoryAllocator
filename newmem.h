#ifndef __NEWMEM_H_
#define __NEWMEM_H_
	int Mem_Init(int sizeOfRegion);
	void * Mem_Alloc(int size);
	int Mem_Free(void * ptr);
	void Mem_Dump();
	int Mem_Available();
#endif
typedef int bool;
#define true 1
#define false 0

typedef struct NodeStruct {
	int size;
	struct NodeStruct *next;
} mem_node;
