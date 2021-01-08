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
	print_num(i);
	return LOW_MEM+(i<<12);
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
