#include "head.h"

struct tss_struct{
  long back_link;	//上一个任务链接  高16位为0
  long esp0;		//0级别内核栈顶
  long ss0;		//0级别栈段选择符 高16位为0
  long esp1;		//1级别
  long ss1;
  long esp2;
  long ss2;
  long cr3;		//页目录寄存器
  long eip;		//指令寄存器
  long eflagss;		//标记寄存器
  long eax;
  long ecx;
  long edx;
  long ebx;
  long esp;
  long ebp;
  long esi;
  long edi;
  long es;		//高16位为0
  long cs;
  long ss;
  long ds;
  long fs;
  long gs;
  long ldt;	        //ldt段选择符 高16位为0
  long trace_bitmap;    //有效位 16-31
};


struct task_struct{
  long state;
  long counter;
  struct desc_struct ldt[3];
  struct tss_struct tss;
};

/*
 *0000 0000 1100 0000  1111 1010 0000 0000
 *0000 0000 0000 0000  0000 0000 1001 1111
 *
 * limit 0-15 16-19   000010011111 (9f+1)*4= 640K
 * base 0x0
 * type 1010  可读可访问
 * DPL 3
 * P 1
 * G 1
 * D 1
 *
 */

#define INIT_TASK \
 { \
    0,15,{{0,0},{0x9f,0xc0fa00},{0x9f,0xc0f200}}, \
    { \
       0,PAGE_SIZE+(long)&init_task,0x10,0,0,0,0,(long)&pg_dir, \
       0,0,0,0,0,0,0,0, \
       0,0,0x17,0x17,0x17,0x17,0x17,0x17, \
       _LDT(0),0x80000000 \
    } \
}

#define FIRST_TSS_ENTRY 4
#define FIRST_LDT_ENTRY (FIRST_TSS_ENTRY+1)
#define _TSS(n) ((((unsigned long) n)<<4)+(FIRST_TSS_ENTRY<<3))
#define _LDT(n) ((((unsigned long) n)<<4)+(FIRST_LDT_ENTRY<<3))
#define ltr(n) __asm__("ltr %%ax"::"a" (_TSS(n)))
#define lldt(n) __asm__("lldt %%ax"::"a" (_LDT(n)))


/*
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

#define NULL 0

static char printbuf[1024];
#define sys_print(buf) \
 __asm__("int $0x80" ::"a"(4),"b"(buf));


static int printf(const char *msg){
    int i=0;
    for(i=0;i<1024;i++){
       printbuf[i] = NULL;
    }	
    i=0;
    while(1==1){
       char temp  = *msg;
       if(!temp){
	 break;
       }
       printbuf[i++] = temp;
       msg++;	
    }	
    sys_print(printbuf);	
}

