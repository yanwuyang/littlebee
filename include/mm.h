#define LOW_MEM 0x100000
#define PAGING_MEMORY (15*1024*1024)
#define PAGING_PAGES (PAGING_MEMORY>>12)
#define MAP_NR(addr) (((addr)-LOW_MEM)>>12)
#define USED 100



#define invalidate() \
__asm__("movl %%eax,%%cr3"::"a" (0))

#define PAGE_DIRTY	0x40
