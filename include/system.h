#define sti() __asm__("sti" ::)

/*
 *                           TSS OR LDT
 *
 * 31----------24-23-22-21-20-19---------16-15-14-13-12-11----8-7-----------0
 * |             |  |  |  |AV|             |  |     |  | TYPE  |            |
 * |  Base 31-24 |G |0 |0 |L | Limit 19-16 |P | DPL |0 |1|0|B|1| Base 23-16 |
 *  ------------------------------------------------------------------------
 * 31------------------------------------16---------------------------------0
 * |                                     |                                  |
 * |       base Address 15~0             |             Limit 15~0           |
 *  -------------------------------------------------------------------------
 *
 *
 */

//movw $104,%1   将段线长设置到gdt描述符表 0~1字节
//movw %%ax,%2   将addr（tss或ldt表地址）低16位 移入描述符2~3字节
//rorl $16,%%eax  将基地址高字节右循环移入 ax中（低字节则进入高字节）
//movb %%al,%3	将基地址16~23位 移入描述符第4字节
//movb $type,%4  将type移入描述符第5字节
//movb $0x00,%%bl  将描述第6字节置0
//movb %%ah,%%bh   将基地址高字节移入描述符第7字节
//rorl $16,%%eax   恢复eax原值
#define _set_tssldt_desc(n,addr,type) \
__asm__ ("movw $104,%1\n\t" \
         "movw %%ax,%2\n\t" \
         "rorl $16,%%eax\n\t" \
         "movb %%al,%3\n\t" \
         "movb $" type ",%4\n\t" \
         "movb $0x00,%%bl\n\t" \
         "movb %%ah,%%bh\n\t" \
         "movw %%bx,%5\n\t" \
         "rorl $16,%%eax" \
	::"a" (addr),"m"(*(n)),"m"(*(n+2)),"m"(*(n+4)),"m"(*(n+5)),"m"(*(n+6)) \
	)

#define set_tss_desc(n,addr) _set_tssldt_desc(((char *) (n)),addr,"0x89")
#define set_ldt_desc(n,addr) _set_tssldt_desc(((char *) (n)),addr,"0x82")

/*
 * ss
 * esp
 * flages
 * cs
 * eip
 *
 */
#define move_to_user_mode() \
__asm__ ("movl %%esp,%%eax\n\t" \
	"pushl $0x17\n\t" \
	"pushl %%eax\n\t" \
	"pushfl\n\t" \
	"pushl $0x0f\n\t" \
	"pushl $1f\n\t" \
	"iret\n" \
	"1:\tmovl $0x17,%%eax\n\t" \
	"mov %%ax,%%ds\n\t" \
	"mov %%ax,%%es\n\t" \
	"mov %%ax,%%fs\n\t" \
	"mov %%ax,%%gs" \
	:::"ax")


#define _set_gate(gate_addr,type,dpl,addr)\
__asm__("movw %%dx,%%ax\n\t" \
    "movw %0,%%dx\n\t" \
    "movl %%eax,%1\n\t" \
    "movl %%edx,%2" \
    : \
    : "i"((short)(0x8000+(dpl<<13)+(type<<8))),\
      "o"(*((char *)(gate_addr))),\
      "o"(*(4+(char *)(gate_addr))),\
      "d" ((char *) (addr)),"a" (0x00080000))

#define set_intr_gate(n,addr) \
	_set_gate(&idt[n],14,0,addr)

#define set_trap_gate(n,addr) \
	_set_gate(&idt[n],15,0,addr)

#define set_system_gate(n,addr) \
	_set_gate(&idt[n],15,3,addr)
