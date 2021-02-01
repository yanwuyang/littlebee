#include "head.h"

#define PAGE_SIZE 4096
#define NR_TASKS 64
#define TASK_SIZE	0x04000000   //64KB

#define HZ 100
#define LATCH (1193180/HZ)

/*
 *tss（task state struct）结构定义参考对应的CPU架构
 */
struct tss_struct {
	long back_link;	//上一个任务链接  高16位为0
	long esp0;		//0级别内核栈顶
	long ss0;		//0级别栈段选择符 高16位为0
	long esp1;		//1级别
	long ss1;
	long esp2;
	long ss2;
	long cr3;		//页目录寄存器
	long eip;		//指令寄存器
	long eflags;		//标记寄存器
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

struct task_struct {
	long pid;
	long state;
	long counter;
	struct desc_struct ldt[3];
	struct tss_struct tss;
};


#define FIRST_TSS_ENTRY 4
#define FIRST_LDT_ENTRY (FIRST_TSS_ENTRY+1)
#define _TSS(n) ((((unsigned long) n)<<4)+(FIRST_TSS_ENTRY<<3))
#define _LDT(n) ((((unsigned long) n)<<4)+(FIRST_LDT_ENTRY<<3))
#define ltr(n) __asm__("ltr %%ax"::"a" (_TSS(n)))
#define lldt(n) __asm__("lldt %%ax"::"a" (_LDT(n)))

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
 * back_link=0
 * esp0=PAGE_SIZE+(long)&init_task
 * ss0=0x10
 * esp1=0		//1级别
 * ss1=0
 * esp2=0
 * ss2=0
 * cr3=(long)&pg_dir
 * eip=0
 * eflags=0
 * eax=0
 * ecx=0
 * edx=0
 * ebx=0
 * esp=0
 * ebp=0
 * esi=0
 * edi=0
 * es=0x17		//高16位为0
 * cs=0x17
 * ss=0x17
 * ds=0x17
 * fs=0x17
 * gs=0x17
 * ldt=_LDT(0)	        //ldt段选择符 高16位为0
 * trace_bitmap=0x80000000
 */

#define INIT_TASK \
 { \
   .pid = 0, \
   .state = 0, \
   .counter = 15, \
   .ldt = { \
        {0,0}, \
        {0x9f,0xc0fa00}, \
        {0x9f,0xc0f200} \
    }, \
   .tss = { \
       .back_link = 0, \
       .esp0 = PAGE_SIZE+(long)&init_task, \
       .ss0 = 0x10, \
       .esp1 = 0, \
       .ss1 = 0, \
       .esp2 = 0, \
       .ss2 = 0, \
       .cr3 = (long)&pg_dir, \
       .eip = 0, \
       .eflags = 0, \
       .eax = 0, \
       .ecx = 0, \
       .edx = 0, \
       .ebx = 0, \
       .esp = 0, \
       .ebp = 0, \
       .esi = 0, \
       .edi = 0, \
       .es = 0x17, \
       .cs = 0x17, \
       .ss = 0x17, \
       .ds = 0x17, \
       .fs = 0x17, \
       .gs = 0x17, \
       .ldt = _LDT(0), \
       .trace_bitmap = 0x80000000 \
    } \
}

#define NULL 0

extern struct task_struct *task[NR_TASKS];
extern struct task_struct *current;

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



#define _set_base(addr,base) \
__asm__("movw %%dx,%0\n\t" \
        "rorl $16,%%edx\n\t" \
        "movb %%dl,%1\n\t" \
        "movb %%dh,%2" \
	::"m" (*((addr)+2)), \
          "m" (*((addr)+4)), \
          "m" (*((addr)+7)), \
          "d" (base))

#define _set_limit(addr,limit) \
__asm__("movw %%dx,%0\n\t" \
	"rorl $16,%%edx\n\t" \
	"movb %1,%%dh\n\t" \
	"andb $0xf0,%%dh\n\t" \
	"orb %%dh,%%dl\n\t" \
	"movb %%dl,%1" \
	::"m" (*(addr)), \
	  "m" (*((addr)+6)), \
	  "d" (limit) \
	:"dx")

#define set_base(ldt,base) _set_base( ((char *)&(ldt)) , base )
#define set_limit(ldt,limit) _set_limit( ((char *)&(ldt)) , (limit-1)>>12 )

#define _get_base(addr) ({\
unsigned long __base; \
__asm__("movb %3,%%dh\n\t" \
	"movb %2,%%dl\n\t" \
	"shll $16,%%edx\n\t" \
	"movw %1,%%dx" \
	:"=d" (__base) \
	:"m" (*((addr)+2)), \
	 "m" (*((addr)+4)), \
	 "m" (*((addr)+7))); \
__base;})

#define get_base(ldt) _get_base( ((char *)&(ldt)) )

#define get_limit(segment) ({ \
unsigned long __limit; \
__asm__("lsll %1,%0\n\tincl %0":"=r" (__limit):"r" (segment)); \
__limit;})

