;读取0号磁道0号磁头从第2号扇区开始
disk_load:
	push dx
	
	mov ah,0x02			;BIOS读取磁盘函数
	mov al,dh			;读取扇区数
	mov ch,0x00			;柱面/磁道号
	mov dh,0x00			;磁头0
	mov cl,0x02			;从第二个扇区开始 引导扇区后的第一个扇区

	int 0x13			;引发BIOS中断读取磁盘

	jc disk_error		;判断CF标志如果置1 表示读取错误

	pop dx
	cmp dh,al			;成功读取的扇区数与需要读取的扇区比较
	jne disk_error		;如果不相等表示读取错误
	ret

;读取0号磁道1号磁头从第1号扇区开始
disk_load1:
    push dx

    mov ah,0x02     	;BIOS读取磁盘函数
    mov al,dh       	;读取扇区数
    mov ch,0x00     	;第0个磁道
    mov dh,0x01     	;磁头1
    mov cl,0x01     	;从第1个扇区开始 引导扇区后的第一个扇区

    int 0x13        	;引发BIOS中断读取磁盘

    jc disk_error   	;判断CF标志如果置1 表示读取错误

    pop dx
    cmp dh,al       	;成功读取的扇区数与需要读取的扇区比较
    jne disk_error  	;如果不相等表示读取错误
    ret

;读取1号磁道0号磁头从第1号扇区开始
disk_load2:
    push dx

    mov ah,0x02     	;BIOS读取磁盘函数
    mov al,dh       	;读取扇区数
    mov ch,0x01     	;第0个磁道
    mov dh,0x00     	;磁头0
    mov cl,0x01    		;从第1个扇区开始 引导扇区后的第一个扇区

    int 0x13        	;引发BIOS中断读取磁盘

    jc disk_error   	;判断CF标志如果置1 表示读取错误

    pop dx
    cmp dh,al       	;成功读取的扇区数与需要读取的扇区比较
    jne disk_error 		 ;如果不相等表示读取错误
    ret


disk_error:
	mov bx,DISK_ERROR_MSG
	call print_string
	jmp $

DISK_ERROR_MSG db "Disk read error!",0
	
	
