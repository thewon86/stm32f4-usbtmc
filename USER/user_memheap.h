#ifndef __USER_MEMHEAP_H__
#define __USER_MEMHEAP_H__

typedef struct memheap MemHeap;

int memheap_init(MemHeap *memheap, void* addr, size_t size);
void *memheap_alloc(MemHeap *heap, size_t size);
void *memheap_realloc(MemHeap *heap, void *ptr, size_t newsize);
void memheap_free(void *ptr);

void initOnChipMemoryHeap(void* addr, size_t size);
void initSdramMemoryHeap(void* addr, size_t size);

void *mallocE(size_t size);
void *callocE(size_t count, size_t size);
void *reallocE(void *addr, size_t size);
void freeE(void *addr);

#endif // __USER_MEMHEAP_H__
