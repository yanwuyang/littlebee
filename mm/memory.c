#include "../include/screen.h"
#include "../include/mm.h"

unsigned char mem_map[PAGING_PAGES] = {0,};

/**
 * 获取空闲页
 */
unsigned long get_free_page(){
	int i=0;
	while(mem_map[i]!=0){
		i++;
	}
	mem_map[i]++;
	return LOW_MEM+(i<<12);
}

int copy_page_tables(unsigned long from,unsigned long to,long size)
{
	unsigned long * from_page_table;
	unsigned long * to_page_table;
	unsigned long this_page;
	unsigned long * from_dir, * to_dir;
	unsigned long new_page;
	unsigned long nr;

	if ((from&0x3fffff) || (to&0x3fffff))
		print("copy_page_tables called with wrong alignment\n");
	from_dir = (unsigned long *) ((from>>20) & 0xffc); /* _pg_dir = 0 */
	to_dir = (unsigned long *) ((to>>20) & 0xffc);
	size = ((unsigned) (size+0x3fffff)) >> 22;
	for( ; size-->0 ; from_dir++,to_dir++) {
		if (1 & *to_dir)
			print("copy_page_tables: already exist\n");
		if (!(1 & *from_dir))
			continue;
		from_page_table = (unsigned long *) (0xfffff000 & *from_dir);
		if (!(to_page_table = (unsigned long *) get_free_page()))
			return -1;	/* Out of memory, see freeing */
		*to_dir = ((unsigned long) to_page_table) | 7;
		nr = (from==0)?0xA0:1024;
		for ( ; nr-- > 0 ; from_page_table++,to_page_table++) {
			this_page = *from_page_table;
			if (!this_page)
				continue;
			if (!(1 & this_page)) {
				/*if (!(new_page = get_free_page()))
					return -1;
				read_swap_page(this_page>>1, (char *) new_page);
				*to_page_table = this_page;
				*from_page_table = new_page | (PAGE_DIRTY | 7);
				continue;*/
			}
			this_page &= ~2;//置位可读可执行,注意：如果对页面写会发生页异常中断，需要对中断进行处理
			*to_page_table = this_page;
			if (this_page > LOW_MEM) {
				*from_page_table = this_page;
				this_page -= LOW_MEM;
				this_page >>= 12;
				mem_map[this_page]++;
			}
		}
	}
	invalidate();
	return 0;
}

void mem_init(long start_mem, long end_mem){
	//将所有页标记为使用
	int i;
	for(i=0;i<PAGING_PAGES;i++){
		mem_map[i]=USED;
	}
	//起始页数
	i = MAP_NR(start_mem);
	//计算空闲页数量 一页4KB
	long pages = (end_mem -start_mem)>>12;
	print("page:");
	print_num(pages);
	print("\n");

	while(pages-- >0){
		mem_map[i++]=0;
	}

}
