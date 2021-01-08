[org 0x7c00]
SYSTEM_SIZE equ	21 * 1024					;系统大小单位字节
LOAD_SECTOR_NUM  equ SYSTEM_SIZE / 512				;需要加载的扇区数

TRACK_SECTOR_NUM equ 18						;单条磁道扇区数
START_SECTOR_NUM equ 2						;从第二个扇区开始 引导扇区后的第一个扇区

KERNEL_OFFSET_SEG equ 0x800					;内核加载到内存中的偏移量
KERNEL_OFFSET equ KERNEL_OFFSET_SEG << 4

DRIVE_INFO equ 0x7f00						;驱动器存储信息
EXT_MEM_K equ 0x7f05						;扩展内存
start:
	mov [BOOT_DRIVE],dl             			;引导扇区所在的磁盘设备
	
	;获取磁盘配置
	;xor dl,dl
	mov ah,0x08
	int 0x13
   	mov [DRIVE_INFO],bl					;驱动器类型0x01-360k 0x02-1.2M 0x03-720k  ->  0x04-1.44M
	mov [DRIVE_INFO+1],ch					;最大磁道号的低8位    -> 0x4f   80个磁道
	mov [DRIVE_INFO+2],cl					;扇区数0-5位 柱面数高2位6-7 ->0x12 每条磁道18个扇区
	mov [DRIVE_INFO+3],dh					;磁头数 -> 0x01  从0开始  2个磁头
	mov [DRIVE_INFO+4],dl                   		;驱动器数 -> 0x01
        							;总容量 2*80*18*512 =
	;获取1M以后的扩展内存
	mov ah,0x88
        int 0x15                                                ;利用BIOS中断0x15 功能号 0x88 读取1MB以后的扩展内存大小（单位KB）
        mov [EXT_MEM_K],ax
	
	mov bp,0xff00						;设置栈基址
	mov sp,bp						;将栈顶与栈基址设置相同内存位置

	mov bx,MSG_REAL_MODE
    call print_string

	call load_kernel

	;call switch_to_pm
    call KERNEL_OFFSET		       				;跳到内核代码处执行
	jmp $


%include "boot/print_string.asm"
;%include "boot/disk_load.asm"

load_kernel:
	mov bx,MSG_LOAD_KERNEL
	call print_string

	mov bx,KERNEL_OFFSET_SEG
    mov es,bx

    read:
		mov ax,[read_sector]			;已读取的扇区数
		mov bx,512
		mul bx
	    mov bx,ax						;内核加载位置

		mov ah,0x02						;BIOS读取磁盘函数
		mov al,[sector]					;读取扇区数
		mov ch,[track]					;柱面/磁道号
		mov dh,[head]					;磁头0
		mov cl,[start_sector]			;开始扇区引导扇区后的第一个扇区
		mov dl,[BOOT_DRIVE]           		 ;磁盘驱动
		int 0x13						;引发BIOS中断读取磁盘
		jc disk_error					;判断CF标志如果置1 表示读取错误

		;cmp dh,al						;成功读取的扇区数与需要读取的扇区比较
		;jne disk_error					;如果不相等表示读取错误
		
		mov dl,[read_sector]
		add al,dl
		mov [read_sector],al			;记录已读取的扇区数量
		
		mov bl,LOAD_SECTOR_NUM
		cmp bl,al				;如果需要读取的扇区数量与已读取的扇区数量不相等则继续读磁盘
		je ok

		mov al,LOAD_SECTOR_NUM
		sub al,[read_sector]			;判断还需要读取的扇区数量
		cmp al,TRACK_SECTOR_NUM			;大于则设置18个扇区
		ja  ok2
		mov [sector],al					;所需读取的扇区
		jmp ok3
	ok2:
		mov al,TRACK_SECTOR_NUM
		mov [sector],al					;所需读取的扇区
	ok3:
		mov dh,[head]
		cmp dh,0						;如果当前磁头是1 则使用磁头0读取,并且磁道加1
		jne  ok4
		mov al,1
		mov [head],al
		jmp ok5
	ok4:
		mov al,0
		mov [head],al					;当前磁头是0,则使用磁头1读取
		mov al,[track]
		add al,1
		mov [track],al
	ok5:
		mov al,1
		mov [start_sector],al
		jmp read

    ;mov bx,KERNEL_OFFSET_SEG
    ;mov es,bx
    ;mov bx,0x0
	;mov dh,17						;在引导盘上加载前15个扇区排除引导扇区
	;mov dl,[BOOT_DRIVE]			;磁盘驱动
	;call disk_load

	;mov bx,0xa20
    ;mov es,bx
    ;mov bx,0x0            			;从磁盘读取数据到内存的位置
    ;mov dh,18                      ;在引导盘上加载前15个扇区排除引导扇区
    ;mov dl,[BOOT_DRIVE]            ;磁盘驱动
    ;call disk_load1

    ;mov bx,0xc60
    ;mov es,bx
    ;mov bx,0x0            			;从磁盘读取数据到内存的位置
    ;mov dh,8                       ;在引导盘上加载前15个扇区排除引导扇区
    ;mov dl,[BOOT_DRIVE]            ;磁盘驱动
    ;call disk_load2
	ok:
	ret

disk_error:
	mov bx,DISK_ERROR_MSG
	call print_string
	jmp $


head db 0		;记录当前磁头
track db 0 		;记录当前磁道
sector db TRACK_SECTOR_NUM - START_SECTOR_NUM + 1 	 ;当前扇区所需读的扇区数量
start_sector db START_SECTOR_NUM
read_sector db 0

BOOT_DRIVE db 0
MSG_REAL_MODE db "Started in 16-bit Real Mode",0
MSG_LOAD_KERNEL db "Loading kernel into memory",0
DISK_ERROR_MSG db "Disk read error!",0

;$ 表示当前指令地址
;$$ 当前程序起始地址 也就是 0x7c00
times 510-($-$$) db 0
dw 0xaa55
