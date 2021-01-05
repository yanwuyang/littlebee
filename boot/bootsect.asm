[org 0x7c00]
KERNEL_OFFSET equ 0x8000			;内核加载到内存中的偏移量
	mov [BOOT_DRIVE],dl             ;引导扇区所在的磁盘设备
	mov ax,0x800
	mov es,ax
	mov ah,0x88
	int 0x15						;利用BIOS中断0x15 功能号 0x88 读取1MB以后的扩展内存大小（单位KB）
	mov [es:2],ax	
    mov [es:6],dl

	;xor dl,dl
	mov ah,0x08
	int 0x13
	mov ax,0x800
	mov es,ax
    mov [es:12],bl					;驱动器类型0x01-360k 0x02-1.2M 0x03-720k  ->  0x04-1.44M
	mov [es:13],ch					;最大磁道号的低8位    -> 0x4f   80个磁道
	mov [es:14],cl					;扇区数0-5位 柱面数高2位6-7 ->0x12 每条磁道18个扇区
	mov [es:15],dh					;磁头数 -> 0x01  从0开始  2个磁头
	mov [es:17],dl                  ;驱动器数 -> 0x01
        							;总容量 2*80*18*512 =
	xor ax,ax
	mov es,ax

	;mov [BOOT_DRIVE],dl			;引导扇区所在的磁盘设备
	
	mov bp,0xff00					;设置栈基址
	mov sp,bp						;将栈顶与栈基址设置相同内存位置

	mov bx,MSG_REAL_MODE
    call print_string

	call load_kernel

	;call switch_to_pm
    call KERNEL_OFFSET              ;跳到内核代码处执行
	jmp $


%include "boot/print_string.asm"
%include "boot/disk_load.asm"

load_kernel:
	mov bx,MSG_LOAD_KERNEL

	call print_string
    mov bx,0x800
    mov es,bx
    mov bx,0x0
	;mov bx,KERNEL_OFFSET			;从磁盘读取数据到内存的位置
	mov dh,17						;在引导盘上加载前15个扇区排除引导扇区
	mov dl,[BOOT_DRIVE]				;磁盘驱动
	call disk_load

    ;mov bx,MSG_LOAD_KERNEL
    ;call print_string
	mov bx,0xa20
    mov es,bx
    mov bx,0x0            			;从磁盘读取数据到内存的位置
    mov dh,18                       ;在引导盘上加载前15个扇区排除引导扇区
    mov dl,[BOOT_DRIVE]             ;磁盘驱动
    call disk_load1

    mov bx,0xc60
    mov es,bx
    mov bx,0x0            			;从磁盘读取数据到内存的位置
    mov dh,8                        ;在引导盘上加载前15个扇区排除引导扇区
    mov dl,[BOOT_DRIVE]             ;磁盘驱动
    call disk_load2

	ret

BOOT_DRIVE db 0
MSG_REAL_MODE db "Started in 16-bit Real Mode",0
MSG_LOAD_KERNEL db "Loading kernel into memory",0

;$ 表示当前指令地址
;$$ 当前程序起始地址 也就是 0x7c00
times 510-($-$$) db 0
dw 0xaa55
