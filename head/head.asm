global switch_to_pm,idt,gdt
[bits 16]

switch_to_pm:
    cli                             ;屏蔽所有中断
    call setup_gdt
    call setup_idt
    mov eax,cr0
    or  eax,0x1
    mov cr0,eax                     ;cr0寄存器第一位置1开启保护模式

    jmp CODE_SEG:init_pm            ;长跳 将代码段索引加载到CS寄存器


;切换到保护模式后 需要重新中断请求号与中断向量的映射关系
;计算机启动后，主片的中断向量为0x08~0x0f；从片的中断向量是0x70~0x77,
;在以8086为处理器的系统是没问题的，在32位处理器上，0x08~0x0f已被处理器用作异常向量
;8259A是课编程的，允许重新设置中断向量。根据Intel公司的建议，中断向量0x20~0xFF（32~255）用户可以自由分配。
;我们可以设置8259A的主片，把它的中断向量改成0x20~0x27
;对8259A编程需要使用初始化命令字（Initizlize Command Word，ICW），设置它的工作方式，共有4个初始化命令字。分配是ICW1~ICW4，都是单字节命令。
;ICW1 设置中断请求的触发方式以及级联芯片的数量    ICW1的位0决定了是否有ICW4 位1是否多片级联
;ICW2 设置每个芯片的中断向量
;ICW3 用于指定用哪个引脚实现芯片的级联
;ICW4 用于控制芯片的工作方式
;主片端口 0x20，0x21
;从片端口 0xA0, 0xA1

;对屏蔽的中断号需要在用到的地方打开

;对8259A芯片执行完ICW以后，在工作过程中可以对它进行操作OCW（Operate Command Word）
;OCW1 用于对8259A中中断屏蔽寄存器IMR进行读/写操作  操作端口 0x21、0xA1
;OCW2 用于发送EOI命令设置或设置中断优先级的自动循环方式。当位4位3==00 操作端口0x20、0xA0
;OCW3 用于设置特殊屏蔽方式和读取寄存器状态（IRR和ISR），当位4位3=01 操作端口0x20、0xA0

setup_idt:
    mov al,0x11		            ;00010001  位0： 需要发送ICW4 位1：有多片级联 位4： 边缘触发
    out 0x20,al                 ;向主片发送ICW1
    ;dw 0x00eb,0x00eb
    out 0xA0,al		            ;向从片发送ICW1

	;设置ICW2
	;dw 0x00eb,0x00eb
	mov al,0x20                 ;主片中断向量起始号0x20
 	out 0x21,al
	;dw 0x00eb,0x00eb
	mov al,0x28		            ;从片中断向量起始号0x28
	out 0xA1,al

	;设置ICW3
	;dw 0x00eb,0x00eb	
	mov al,0x04		            ;主芯片R2连从芯片INT
	out 0x21,al
	;dw 0x00eb,0x00eb
	mov al,0x02		            ;
	out 0xA1,al
	
    ;设置ICW4
	;dw 0x00eb,0x00eb
	mov al,0x01          	     ;位2：AEOI 为0时非自动结束，要求中断处理过程中明确向8259A芯片写中断结束命令EOI 为1时 表示自动结束
	out 0x21,al
	;dw 0x00eb,0x00eb
	out 0xA1,al

	;屏蔽的中断向量
	;dw 0x00eb,0x00eb
	mov al,0xff
	out 0x21,al		              ;屏蔽主芯片所有中断请求
	;dw 0x00eb,0x00eb
	out 0xA1,al		              ;屏蔽从芯片所有中断请求

	
 	lidt [idt_descr] 
	ret    


setup_gdt:
   lgdt [gdt_descriptor]
   ret
gdt:
gdt_null:                       ;定义空描述符
    dd 0x0                      ;定义4个字节的0
    dd 0x0                      ;定义4个字节的0

gdt_code:
    dw 0xffff                   ;limit(bits 0-15)
    dw 0x0                      ;基址(bits 0-15)
    db 0x0                      ;基址(bits 16-23)
    db 10011010b                ;1st flags,type flags
    db 11001111b                ;2nd flags,Limit(bits 16-19)
    db 0x0                      ;基址(bits 24-31)

gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

zero times 253*8 db 0

gdt_end:

gdt_descriptor:                 ;GDT段描述符
    dw gdt_end-gdt-1            ;段大小2个字节
    dd gdt                      ;段基址4个字节

CODE_SEG equ gdt_code - gdt     ;代码段索引
DATA_SEG equ gdt_data - gdt     ;数据段索引


idt:
 times 256*8 db 0

idt_descr:
  dw 256*8-1
  dd idt


[bits 32]

[extern after_setup_page]
init_pm:

    mov ax,DATA_SEG
    mov ds,ax                  ;设置数据段寄存器
    mov ss,ax
    mov es,ax
    mov fs,ax
    mov gs,ax

    mov ebp,0x90000
    mov esp,ebp

    jmp after_setup_page


