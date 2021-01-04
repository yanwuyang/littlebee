[bits 32]				;编译为32位代码

VIDEO_MEMORY equ 0xb8000+2*(14*80)		;显示内存的开始地址
WHITE_ON_BLACK equ 0x0f			;文本模式

print_string_pm:			
	pusha				;将所有寄存器值压入栈
	mov edx,VIDEO_MEMORY		;设置显示内存的开始地址

print_string_pm_loop:
	mov al,[ebx]			;将第一个字符设置给al	
	mov ah,WHITE_ON_BLACK		;设置显示模式

	cmp al,0			;比较字符是否==0 如果等于0表示打印结束
	je print_string_pm_done

	mov [edx],ax			;将字符和字符属性存储到视频显示的位置

	add ebx,1			;将字符地址加1
	add edx,2			;将显示地址加1	

	jmp print_string_pm_loop

print_string_pm_done:
	popa
	ret
