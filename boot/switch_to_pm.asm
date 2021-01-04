[bits 16]

switch_to_pm:

	cli				;屏蔽所有中断
	lgdt [gdt_descriptor]		;加载gdt描述符
	
	mov eax,cr0			
	or  eax,0x1
	mov cr0,eax			;cr0寄存器第一位置1开启保护模式

	jmp CODE_SEG:init_pm		;长跳 将代码段索引加载到CS寄存器

[bits 32]


init_pm:
        	
	mov ax,DATA_SEG
	mov ds,ax			;设置数据段寄存器
	mov ss,ax
	mov es,ax
	mov fs,ax
	mov gs,ax

	mov ebp,0x90000
	mov esp,ebp
 
	call BEGIN_PM



