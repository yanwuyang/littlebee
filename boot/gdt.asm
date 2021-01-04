gdt_start:
	
gdt_null:			;定义空描述符
	dd 0x0			;定义4个字节的0
	dd 0x0			;定义4个字节的0

gdt_code:
	dw 0xffff		;limit(bits 0-15)
	dw 0x0			;基址(bits 0-15)
	db 0x0			;基址（bits 16-23)
	db 10011010b		;1st flags,type flags
	db 11001111b		;2nd flags,Limit(bits 16-19)
	db 0x0			;基址(bits 24-31)

gdt_data:
	dw 0xffff
	dw 0x0
	db 0x0
	db 10010010b
	db 11001111b
	db 0x0

zero times  40 dd 0

gdt_end:

gdt_descriptor:			;GDT段描述符
	dw gdt_end-gdt_start-1	;段大小2个字节
	dd gdt_start		;段基址4个字节

CODE_SEG equ gdt_code - gdt_start ;代码段索引
DATA_SEG equ gdt_data - gdt_start ;数据段索引
	
