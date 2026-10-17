#include "memory.h"
#include "system.h"
#include <dolphin/os.h>
#include <string.h>

//HEAP_MALLOC block is a series of these nodes
typedef struct MapAllocEntry MapAllocEntry;
struct MapAllocEntry {
	MapAllocEntry* next; //0x0
	u32 size; //0x4
	u16 inuse; //0x8
	u16 currId; //0xA
	u8 field_0xC[0x20 - 0xC]; //0xC
};

//.bss
__declspec(section ".data") u8 gap_06_803D48AC_bss[4];
#pragma force_active on
SmartWork smartWork;
#pragma force_active reset

//.sdata
//flag, size / 0x400 (>> 10)
s32 size_table[6][2] = {
	{1, 0x1C84}, //0x721000, 7300 KiB/7.13 MiB
	{1, 0xD90}, //0x364000, 3472 KiB/3.39 MiB
	{1, 0xE0}, //0x38000, 224 KiB
	{1, 0x80}, //0x20000, 128 KiB
	{1, 0x100}, //0x40000, 256 KiB
	{0, 0x64}
};
static SmartWork* wp = &smartWork;
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_8040F50C_sdata[1] = {0};
#pragma force_active reset

//.sbss
OSHeapHandle heapHandle[6];
void* heapStart[6];
void* heapEnd[6];
void* mapalloc_base_ptr;
u32 mapalloc_size;
BOOL g_bFirstSmartAlloc;
u32 count;
#define COUNT (*(u16*)&count)

//local prototypes
void smartGarbage(void);
extern OSThread dvdmgr_thread;

void memInit(void) { //almost 1:1, stubborn regalloc at the end
    MapAllocEntry* entry;
    void* arenaLo;
    void* arenaHi;
    u32 temp_r31;
    int i;
    int size;
    u32 temp;

    arenaLo = OSGetArenaLo();
    arenaHi = OSGetArenaHi();
    arenaLo = OSInitAlloc(arenaLo, arenaHi, 6);
    OSSetArenaLo(arenaLo);
    arenaLo = (void*)OSRoundUp32B(arenaLo);
    arenaHi = (void*)OSRoundDown32B(arenaHi);
    temp = (u32)arenaLo;

    for (i = 0; i < 6; i++) {
        if (size_table[i][0] == 1) {
            heapStart[i] = (void*)temp;
            size = size_table[i][1] << 10;
            heapEnd[i] = (void*)(temp + size);
            temp += size;
        }
    }

    temp_r31 = (u32)arenaHi - temp;
    for (i = 0; i < 6; i++) {
        if (size_table[i][0] == 0) {
            size = (size_table[i][1] * (u64)temp_r31) / 100Ull;
            heapStart[i] = (void*)temp;
            size -= (size & 0x1F);
            heapEnd[i] = (void*)(temp + size);
            temp += size;
        }
    }
    for (i = 0; i < 6; i++) {
        heapHandle[i] = OSCreateHeap(heapStart[i], heapEnd[i]);
    }
    OSSetArenaLo((void* ) arenaHi);
    for (i = 0; i < 6; i++) {
        memClear(i);
    }
}

void* __memAlloc(HEAP_TYPE heap, u32 size);

void memClear(HEAP_TYPE heap) {
	MapAllocEntry* entry;
	u32 size;

	size = (u32)heapHandle[heap];
	OSDestroyHeap((OSHeapHandle)size);
	OSCreateHeap(heapStart[heap], heapEnd[heap]);
	if (heap == HEAP_MALLOC) {
		size = (u32)heapEnd[1] - (u32)heapStart[1] - 0x20;
		entry = OSAllocFromHeap(heapHandle[HEAP_MALLOC], size);
		if (entry) {
			memset(entry, 0, size);
			DCFlushRange(entry, size);
		}
		entry->next = NULL;
		entry->size = size - 0x20;
		entry->inuse = 0;
		mapalloc_base_ptr = entry;
		mapalloc_size = size;
		COUNT = 0;
	}
}

void* __memAlloc(HEAP_TYPE heap, u32 size) {
	void* ptr;

	ptr = OSAllocFromHeap(heapHandle[heap], size);
	if (ptr) {
		memset(ptr, 0, size);
		DCFlushRange(ptr, size);
	}
	return ptr;
}

void __memFree(HEAP_TYPE heap, void* ptr) {
	OSFreeToHeap(heapHandle[heap], ptr);
}

void* _mapAlloc(u32 size) {
	MapAllocEntry* nextptr; //shorthand to make sure regalloc is correct
	MapAllocEntry* entry; //for our loop
	MapAllocEntry* tailEntry; //make sure we don't lose when inserting, might need better name
	MapAllocEntry* currEntry; //one we're allocating
	void* usableData; //memory caller can actually use
	u32 usableSize; //size minus 0x20 header
	u32 maxSize; //used to update latest tail
	u32 fullSize; //size including header

	usableSize = OSRoundUp32B(size);
	if (!usableSize) {
		return NULL;
	}
	entry = mapalloc_base_ptr;
	fullSize = usableSize + 0x20;
	maxSize = 0;
	tailEntry = 0;
	while (1) {
		if (!entry->inuse && entry->size >= fullSize) {
			if (maxSize > entry->size || !maxSize) {
				tailEntry = entry->next;
				currEntry = entry;
				maxSize = entry->size;
			}
		}
		if (!entry->next) break;
		entry = entry->next;
	}
	if (maxSize) {
		usableData = (void*)((u32)currEntry + 0x20);

		currEntry->next = (void*)((u32)currEntry + fullSize);
		currEntry->size = usableSize;
		currEntry->inuse = 1;
		currEntry->currId = COUNT++;

		nextptr = currEntry->next;
		nextptr->next = tailEntry;
		nextptr->size = maxSize - fullSize;
		nextptr->inuse = 0;
		nextptr->currId = 0;

		memset(usableData, 0, usableSize);
		DCFlushRange(usableData, usableSize);
		return usableData;
	}
	else {
		return NULL;
	}
}

void* _mapAllocTail(u32 size) {
	MapAllocEntry* nextptr; //shorthand to make sure regalloc is correct
	MapAllocEntry* entry; //for our loop
	MapAllocEntry* tailEntry; //make sure we don't lose when inserting, might need better name
	MapAllocEntry* currEntry; //one we're allocating
	//void* usableData; //memory caller can actually use
	u32 maxSize; //used to update latest tail
	u32 fullSize; //size including header
	u32 usableSize; //size minus 0x20 header

	usableSize = OSRoundUp32B(size);
	if (!usableSize) {
		return NULL;
	}
	entry = mapalloc_base_ptr;
	fullSize = usableSize + 0x20;
	maxSize = 0;
	tailEntry = 0;
	while (1) {
		if (!entry->inuse && entry->size >= fullSize) {
			tailEntry = entry->next;
			currEntry = entry;
			maxSize = entry->size;
		}
		if (!entry->next) break;
		entry = entry->next;
	}
	if (maxSize) {
		memset(currEntry, 0, fullSize);
		DCFlushRange(currEntry, fullSize);

		currEntry->next = (MapAllocEntry*)(((u32)currEntry + maxSize) - fullSize);
		currEntry->size = maxSize - fullSize;
		currEntry->inuse = 0;
		currEntry->currId = 0;

		nextptr = currEntry->next;
		nextptr->next = tailEntry;
		tailEntry = (void*)((u32)nextptr + 0x20);
		nextptr->size = usableSize;
		nextptr->inuse = 1;
		nextptr->currId = COUNT++;

		memset(tailEntry, 0, usableSize);
		DCFlushRange(tailEntry, usableSize);
		return tailEntry;
	}
	else {
		return NULL;
	}
}

void _mapFree(void* alloc) {
	MapAllocEntry* loopEntry;
	MapAllocEntry* nextEntry;
	u32 newSize;
	MapAllocEntry* entry;

	if (!alloc) return;
	entry = (MapAllocEntry*)((u32)alloc - 0x20);
	if (!entry->inuse) return;
	nextEntry = entry->next;
	newSize = entry->size;
	if (nextEntry && !nextEntry->inuse) {
		newSize += nextEntry->size + 0x20;
		nextEntry = nextEntry->next;
	}
	loopEntry = mapalloc_base_ptr;
	while (1) {
		if (loopEntry->next == entry) {
			if (!loopEntry->inuse) {
				newSize += loopEntry->size + 0x20;
				entry = loopEntry;
			}
			break;
		}
		if ((u32)loopEntry > (u32)entry || !loopEntry->next) break;
		loopEntry = loopEntry->next;
	}
	entry->next = nextEntry;
	entry->size = newSize;
	entry->inuse = 0;
}

void smartInit(void) {
	smartEntry* e;
	u32 size;
	void* ptr;
	int i;

	size = (u32)heapEnd[5] - (u32)heapStart[5] - 32;
	ptr = OSAllocFromHeap(heapHandle[5], size);
	if (ptr) {
		memset(ptr, 0, size);
		DCFlushRange(ptr, size);
	}
	wp->ptr = ptr;
	wp->field_0xE008 = 0;
	wp->field_0xE00C = 0;
	wp->bytesLeft = (u32)heapEnd[5] - (u32)heapStart[5] - 32;
	memset(wp->ptr, 0, (u32)heapEnd[5] - (u32)heapStart[5] - 32);
	memset(wp->work, 0, sizeof(wp->work));
	e = wp->work;
	for (i = 0; i < 0x800; i++, e++) {
		e->next = e + 1;
		e->prev = e - 1;
	}
	wp->head = &wp->work[0];
	wp->head->prev = NULL;
	wp->tail = &wp->work[0x7FF];
	wp->tail->next = NULL;
	wp->waitsync = 0;
	g_bFirstSmartAlloc = FALSE;
}

void L_smartReInit(void) {
	smartEntry* e;
	int i;

	wp->field_0xE008 = 0;
	wp->field_0xE00C = 0;
	wp->bytesLeft = (u32)heapEnd[5] - (u32)heapStart[5] - 32;
	memset(wp->ptr, 0, (u32)heapEnd[5] - (u32)heapStart[5] - 32);
	memset(wp->work, 0, sizeof(wp->work));
	e = wp->work;
	for (i = 0; i < 0x800; i++, e++) {
		e->next = e + 1;
		e->prev = e - 1;
	}
	wp->head = &wp->work[0];
	wp->head->prev = NULL;
	wp->tail = &wp->work[0x7FF];
	wp->tail->next = NULL;
	g_bFirstSmartAlloc = 0;
}

#pragma dont_inline on
void smartAutoFree(s32 a1) {
	smartEntry* entry;
	smartEntry* next;
	u16 type = a1;

	for (entry = wp->field_0xE008; entry; entry = next) {
		next = entry->next;
		if (entry->type == type) {
			smartFree(entry);
		}
	}
	if (a1 == 3) {
		for (entry = wp->field_0xE008; entry; entry = next) {
			next = entry->next;
			if ((s32)entry->type == 4) {
				smartFree(entry);
			}
		}
	}
}

#pragma dont_inline reset
void smartFree(smartEntry* entry) {
	u32 total;

	if (entry->type == 4) {
		entry->type = 3;
		return;
	}
	if (!entry->prev) {
		wp->field_0xE008 = entry->next;
		if (wp->field_0xE008) {
			wp->field_0xE008->prev = NULL;
		}
	}
	else {
		entry->prev->next = entry->next;
	}
	if (!entry->next) {
		wp->field_0xE00C = entry->prev;
		if (wp->field_0xE00C) {
			wp->field_0xE00C->next = NULL;
		}
	}
	else {
		entry->next->prev = entry->prev;
	}
	total = entry->size + entry->field_0x10;
	if (entry->prev) {
		entry->prev->field_0x10 += total;
	}
	else {
		wp->bytesLeft += total;
	}
	if (!wp->head) {
		wp->head = entry;
		entry->prev = NULL;
	}
	else {
		wp->tail->next = entry;
		entry->prev = wp->tail;
	}
	entry->field_0xC = 0;
	entry->next = NULL;
	entry->size = 0;
	entry->field_0x10 = 0;
	entry->field_0x8 = NULL;
	wp->tail = entry;
	wp->waitsync++;
}

smartEntry* smartAlloc(u32 size, u8 type) {
	smartEntry* entry1;
	smartEntry* entry2;
	int i;
	u32 sz = size;

	if (!g_bFirstSmartAlloc) {
		g_bFirstSmartAlloc = TRUE;
		smartAutoFree(3);
	}

	if (wp->waitsync) {
		sysWaitDrawSync();
		wp->waitsync = 0;
	}

	entry2 = wp->head;

	if (!entry2->prev) {
		wp->head = entry2->next;
		if (wp->head) {
			wp->head->prev = NULL;
		}
	}
	else {
		entry2->prev->next = entry2->next;
	}

	if (!entry2->next) {
		wp->tail = entry2->prev;
		if (wp->tail) {
			wp->tail->next = NULL;
		}
	}
	else {
		entry2->next->prev = entry2->prev;
	}

	if (sz & 31) {
		sz += 32 - (sz & 31);
	}

	entry2->field_0xC = 1;
	entry2->type = type;
	entry2->size = sz;
	entry2->field_0x8 = 0;
	if (wp->bytesLeft >= sz) {
		entry2->address = wp->ptr;
		entry2->field_0x10 = wp->bytesLeft - sz;
		entry2->next = wp->field_0xE008;
		entry2->prev = NULL;
		wp->bytesLeft = 0;
		if (wp->field_0xE008) {
			wp->field_0xE008->prev = entry2;
		}
		wp->field_0xE008 = entry2;
		if (!entry2->next) {
			entry2->field_0x10 = ((u32)heapEnd[5] - (u32)heapStart[5] - 32) + (u32)wp->ptr - (u32)entry2->address - entry2->size;
			wp->field_0xE00C = entry2;
		}
		return entry2;
	}
	else {
		for (entry1 = wp->field_0xE008; entry1; entry1 = entry1->next) {
			if (entry1->field_0x10 >= sz) {
				entry2->address = (void*)((u32)entry1->address + entry1->size);
				entry2->field_0x10 = entry1->field_0x10 - sz;
				entry2->next = entry1->next;
				entry2->prev = entry1;
				entry1->field_0x10 = 0;
				if (entry1->next) {
					entry1->next->prev = entry2;
				}
				else {
					wp->field_0xE00C = entry2;
				}
				entry1->next = entry2;
				return entry2;
			}
		}
		for (i = 0; i < 3; i++) {
			switch (i) {
				case 0:
					break;
				case 1:
					_fileGarbage(1);
					break;
				case 2:
					_fileGarbage(0);
					break;
			}
			smartGarbage();
			entry1 = wp->field_0xE00C;
			if (entry1->field_0x10 >= sz) {
				entry2->address = (void*)((u32)entry1->address + entry1->size);
				entry2->field_0x10 = entry1->field_0x10 - sz;
				entry2->next = entry1->next;
				entry2->prev = entry1;
				entry1->field_0x10 = 0;
				if (entry1->next) {
					entry1->next->prev = entry2;
				}
				else {
					wp->field_0xE00C = entry2;
				}
				entry1->next = entry2;
				return entry2;
			}
		}
		return NULL;
	}
}

void smartGarbage(void) {
	u8* addr;
	u8* ptr;
	u32 total;
	smartEntry* entry;
	smartEntry* prev;

	sysWaitDrawSync();
	if (wp->field_0xE008) {
		wp->bytesLeft = 0;
	}
	else {
		wp->bytesLeft = (u32)heapEnd[5] - (u32)heapStart[5] - 32;
	}
	prev = NULL;
	ptr = wp->ptr;
	for (entry = wp->field_0xE008; entry; prev = entry, entry = entry->next) {
		addr = entry->address;
		if (addr != ptr) {
			if (entry->field_0x8) {
				if (entry->field_0x8->state == 3 && entry->field_0x8->entry && OSGetCurrentThread() != &dvdmgr_thread) {
					if (prev) {
						prev->field_0x10 = addr - ptr;
					}
					else {
						wp->bytesLeft = addr - ptr;
					}
					ptr = (u8*)entry->address + entry->size;
					entry->field_0x10 = 0;
					continue;
				}
				fileGarbageMoveMem(ptr, entry->field_0x8);
			}
			else {
				memmove(ptr, addr, entry->size);
				entry->address = ptr;
			}
		}
		entry->field_0x10 = 0;
		ptr = (u8*)entry->address + entry->size;
	}
	entry = wp->field_0xE00C;
	if (entry) {
		entry->field_0x10 = ((u32)heapEnd[5] - (u32)heapStart[5] - 32) + (u32)wp->ptr - (u32)entry->address - entry->size;
	}
	DCFlushRange(wp->ptr, (u32)heapEnd[5] - (u32)heapStart[5] - 32);
}

GXTexObj* smartTexObj(GXTexObj* obj, void** image_ptr) {
	if (image_ptr) {
		GXInitTexObjData(obj, *image_ptr);
	}
	return obj;
}
