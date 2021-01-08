#include "../include/screen.h"


void mem_init(long start_mem, long end_mem){
	//计算总共页数 一页4KB
	long pages = (end_mem -start_mem)>>12;
	print("page:");
	print_num(pages);
	print("\n");
}
