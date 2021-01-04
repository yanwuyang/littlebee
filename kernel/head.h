#define PAGE_SIZE 4096
#define NR_TASKS 64

typedef struct desc_struct {
	unsigned long a,b;
} desc_table[256];

extern unsigned long pg_dir[1024];
extern desc_table idt,gdt;
