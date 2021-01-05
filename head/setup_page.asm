;分页设置 线性地址 等于 物理地址
;开启分页需要在开启保护模式之后设置
[bits 32]
global pg_dir,after_setup_page
[extern main]

pg_dir equ 0x0000			;页目录地址
pg0 equ 0x1000				;第一页表地址
pg1 equ 0x2000				;第二页表地址	
pg2 equ 0x3000				;第三页表地址
pg3 equ 0x4000				;第四页表地址


after_setup_page:
	push main			;分页设置后的返回地址
	jmp setup_page			;分页

setup_page:
	mov ecx,1024*5			;对页目录地址以及页表地址5页每页4KB 一页1024项 全清零
	xor eax,eax
	xor edi,edi
	cld
	rep
	stosd				;从0x000地址开始将eax中的值复制到es[edi]中且edi增4  rep stosd dword ptr es:[edi], eax

	
	mov dword [pg_dir],pg0+7	;设置页目录项第一项 第一个页表地址0x1007   0x7=0111 页存在，用户可读可写
	mov dword [pg_dir+4],pg1+7
	mov dword [pg_dir+8],pg2+7
	mov dword [pg_dir+12],pg3+7
	
	mov dword edi,pg3+4092		;填写页表所有项 4个页面 每个页面1024项 一页4kb即4*1024*4kb =16MB 从最后一个页表最后一项开始按倒退顺序填写
	mov eax,0xfff007		;最后一个物理页面地址 0xfff000=16MB-4096   0x7=0111 页存在，用户可读可写  
	
	std				;方向位置位
a:
	stosd				;将eax值赋值到es[edi]中且edi-4
	sub eax,0x1000			;eax-4096->eax中
	jge a				;如果大于等于0 则继续填充
	
	xor eax,eax			;eax置0
	mov cr3,eax			;现在设置页目录表基址寄存器cr3,cr3中保存的是页目录表的物理地址，设置页目录地址0x0000
	mov eax,cr0
	or  eax,0x80000000		;启动分页处理cr0的标志，位31
	mov cr0,eax

	ret
	





