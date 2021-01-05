global keyboard_interrupt,timer_interrupt,system_call,sys_fork
[extern keyboard]
[extern schedule]
[extern sys_call_table]
[extern copy_process]
keyboard_interrupt:
        mov al,0x20
        out 0x20,al             ;设置中断器主片  EOI 1
        call keyboard
        xor eax,eax
        in al, 0x60
        mov al,'a'
        mov ah,3
        ;mov [0xb8000],al
        ;mov [0xb8001],ah

        iret

timer_interrupt:
	push ds
	push es
	push fs		;保存用户空间的ds,es,fs
	push edx
	push ecx
	push ebx
	push eax	;保存edx,ecx,ebx,eax 参数
	mov eax,0x10	;ds、es指向内核数据段
	mov ds,ax
	mov es,ax
	mov fs,ax

        mov al,0x20	;由于初始化中断控制芯片时没有采用自动EOI，所以这里需要发指令结束该硬件中断
        out 0x20,al

        call schedule

	pop eax
	pop ebx
	pop ecx
	pop edx
	pop fs
	pop es
	pop ds
        iret

; ss
; esp
; eflags
; cs
; eip
system_call:
	push ds
	push es
	push fs
	push eax
	push edx
	push ecx
	push ebx
	mov edx,0x10	;内核段
	mov ds,dx
	mov es,dx

        call [sys_call_table+eax] ;eax调用号
	
	push eax	;返回值
	pop eax
	pop ebx
	pop ecx
	pop edx
	add esp,4	;最开始的eax  参数系统调用编号
	pop fs
	pop es
	pop ds
        iret


sys_fork:
 	push gs
	push esi
	push edi
	push ebp
        call copy_process
        add  esp,16   ;忽略 gs,esi,edi,ebp
        ret
